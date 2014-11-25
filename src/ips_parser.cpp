/* 
 * Copyright (c) 2013-2014, Quarkslab
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * - Neither the name of Quarkslab nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <leeloo/config.h>
#include <leeloo/ips_parser.h>
#include <leeloo/ip_list_intervals.h>
#ifdef LEELOO_MP_SUPPORT
#include <leeloo/ipv6_list_intervals.h>
#endif

#ifdef WIN32
#include <Winsock2.h>
// AG: remove these useless macros!!
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <string.h>

// This is a closed interval [min, max]
template <class Integer>
struct closed_interval
{
	typedef Integer integer_type;

#ifndef DEBUG
	closed_interval():
		min(-1),
		max(-1)
	{ }
#endif

	void set(integer_type const min_, uint8_t const max_)
	{
		if (min_ > max_) {
			min = max_;
			max = min_;
		}
		else {
			min = min_;
			max = max_;
		}
	}

	integer_type min;
	integer_type max;
};

typedef closed_interval<uint8_t>  byte_interval;
typedef closed_interval<uint16_t> short_interval;

static size_t count_char_buf(const char* str, size_t size, const char c)
{
	size_t ret = 0;
	for (size_t i = 0; i < size; i++) {
		if (str[i] == c) {
			ret++;
		}
	}
	return ret;
}

static inline uint32_t atoi3(const char* str, const size_t len_int)
{
	static int pow10[] = {1, 10, 100};

	if (len_int == 0 || len_int >= 4) {
		return -1;
	}
	uint32_t cur_int = 0;
	for (size_t j = 0; j < len_int; j++) {
		const char c = str[j];
		if ((c < '0') || (c > '9')) {
			return -1;
		}
		cur_int += (c-'0')*pow10[len_int-j-1];
	}
	return cur_int;
}

static inline uint32_t atoi3_trim(const char* str, const size_t len_int)
{
	if (len_int == 0) {
		return -1;
	}

	size_t i;
	size_t new_len = len_int;
	for (i = 0; i < len_int; i++) {
		if (!isspace(str[i])) {
			break;
		}
		new_len--;
	}

	for (size_t j = len_int-1; j > i; j--) {
		if (!isspace(str[j])) {
			break;
		}
		new_len--;
	}

	return atoi3(&str[i], new_len);
}


uint32_t leeloo::ips_parser::ipv4toi(const char* str, const size_t size, bool& valid, int min_dots)
{
	if (size == 0) {
		valid = false;
		return 0;
	}

	uint32_t ret = 0;
	int cur_idx = 3;
	size_t i;
	for (i = 0; i <= size; i++) {
		if (!isspace(str[i])) {
			break;
		}
	}
	size_t start_idx = i;
	for (; i <= size; i++) {
		const char c = str[i];
		if (c == '.' || (i == size)) {
			if (cur_idx < 0) {
				valid = false;
				return 0;
			}

			const uint32_t cur_int = atoi3_trim(&str[start_idx], i-start_idx);
			if (cur_int > 0xFF) {
				valid = false;
				return 0;
			}
			ret |= cur_int << (8*cur_idx);
			cur_idx--;
			start_idx = i+1;
			continue;
		}

		if (isspace(c)) {
			continue;
		}

		if ((c < '0') || (c > '9')) {
			valid = false;
			return 0;
		}
	}

	valid = (cur_idx <= (2-min_dots));
	return ret;
}

uint32_t leeloo::ips_parser::ipv4toi(const char* str, bool& valid, int min_dots)
{
	return ipv4toi(str, strlen(str), valid, min_dots);
}

template <bool exclude>
static bool __parse_ips(leeloo::ip_list_intervals& l, const char* str)
{
	// We need to support these formats:
	// 10.0.1.0/24
	// 10/8
	// 10.4.9.0
	// 10.4-5.8-11.0-255
	// 10.0.1.4-10.2.4.10
	
	const size_t size_str = strlen(str);

	// TODO: merge these two loops
	const size_t ndashes = count_char_buf(str, size_str, '-');
	const size_t nslashes = count_char_buf(str, size_str, '/');

	// Fast case with no slashes or dashes (simple IP)
	if ((nslashes == 0) && (ndashes == 0)) {
		// Just parse the IP
		bool valid;
		const uint32_t ip = leeloo::ips_parser::ipv4toi(str, size_str, valid);
		if (!valid) {
			return false;
		}

		l.insert<exclude>(ip, ip);
		return true;
	}

	if (ndashes == 1) {
		if (nslashes > 0) {
			return false;
		}

		// We want to parse this:
		// 10.0.1.4-10.2.4.10
		// We support the fact that the interval could have been written the wrong way.
		// (like 10.2.4.10-10.0.1.4)
		const char* sep = strchr(str, '-');
		if (*(sep+1) == 0) {
			return false;
		}
		const size_t idx_sep = (uintptr_t)sep-(uintptr_t)str;

		bool valid = false;
		uint32_t a = leeloo::ips_parser::ipv4toi(str, idx_sep, valid);
		if (!valid) {
			return false;
		}
		const size_t size_part2 = size_str-(idx_sep+1);
		uint32_t b = leeloo::ips_parser::ipv4toi(sep+1, size_part2, valid);
		if (!valid) {
			// Check if this is just a number
			b = atoi3(sep+1, size_part2);
			if (b > 0xFF) {
				return false;
			}
			b = (a & 0xFFFFFF00) | b;
		}
		if (a > b) {
			std::swap(a, b);
		}
		l.insert<exclude>(a, b);
		return true;
	}

	if (nslashes > 1) {
		return false;
	}

	if (ndashes == 0) {
		const char* slash = strchr(str, '/');
		if (!slash || strlen(slash) == 0) {
			return false;
		}
		const int cidr = strtol(slash+1, nullptr, 10);
		if (cidr < 0) {
			return false;
		}
		bool valid = false;
		uint32_t ip_start = leeloo::ips_parser::ipv4toi(str, (uintptr_t)slash-(uintptr_t)str, valid, 0);
		if (!valid) {
			return false;
		}

		l.insert_cidr<exclude>(ip_start, cidr);

		return true;
	}

	byte_interval intervals[4];
	const char* cur = str;
	int cur_interval = 0;
	const char* dot;
	while (((dot = strchr(cur, '.')) != nullptr) && cur_interval < 4) {
		const char* dash = (const char*) memchr(cur, '-', (uintptr_t)dot-(uintptr_t)cur);
		if (dash) {
			const int32_t byte_min = atoi3_trim(cur, (uintptr_t)dash-(uintptr_t)cur);
			const int32_t byte_max = atoi3_trim(dash+1, (uintptr_t)dot-(uintptr_t)(dash+1));
			if (byte_min == -1 || byte_max == -1) {
				return false;
			}
			intervals[cur_interval].set(byte_min, byte_max);
		}
		else {
			const int32_t byte = atoi3_trim(cur, (uintptr_t)dot-(uintptr_t)cur);
			if (byte == -1) {
				return false;
			}
			intervals[cur_interval].set(byte, byte);
		}
		cur = dot+1;
		cur_interval++;
	}

	// Check that 3 intervals have been found
	if (cur_interval != 3) {
		return false;
	}

	// Last one
	const char* dash = strchr(cur, '-');
	if (dash) {
		const int32_t byte_min = atoi3_trim(cur, (uintptr_t)dash-(uintptr_t)cur);
		const int32_t byte_max = atoi3_trim(dash+1, size_str-((uintptr_t)(dash+1)-(uintptr_t)str));
		if (byte_min == -1 || byte_max == -1) {
			return false;
		}
		intervals[cur_interval].set(byte_min, byte_max);
	}
	else {
		const int32_t byte = atoi3_trim(cur, (uintptr_t)dot-(uintptr_t)cur);
		if (byte == -1) {
			return false;
		}
		intervals[cur_interval].set(byte, byte);
	}

	// We got our intervals, let's create them
	for (uint32_t a = intervals[0].min; a <= intervals[0].max; a++) {
		for (uint32_t b = intervals[1].min; b <= intervals[1].max; b++) {
			for (uint32_t c = intervals[2].min; c <= intervals[2].max; c++) {
				const uint32_t ip_base = (a<<24) | (b<<16) | (c<<8);
				const uint32_t ip_min = ip_base | intervals[3].min;
				const uint32_t ip_max = ip_base | intervals[3].max;
				l.insert<exclude>(ip_min, ip_max);
			}
		}
	}

	return true;
}

bool leeloo::ips_parser::parse_ips_add(ip_list_intervals& l, const char* str)
{
	return __parse_ips<false>(l, str);
}

bool leeloo::ips_parser::parse_ips_remove(ip_list_intervals& l, const char* str)
{
	return __parse_ips<true>(l, str);
}

std::string leeloo::ips_parser::ipv4tostr(uint32_t const ip)
{
	uint32_t const ip_n = htonl(ip);
	return inet_ntoa(*reinterpret_cast<struct in_addr const*>(&ip_n));
}

#ifdef LEELOO_MP_SUPPORT

// IPv6 support
//

leeloo::ipv6_int leeloo::ips_parser::ipv6toi(const char* str, const size_t size, bool& valid)
{
	// AG: this a POC, waiting for something better!
	char* tmp_str = (char*) malloc(size+1);
	if (tmp_str == nullptr) {
		valid = false;
		return 0;
	}
	memcpy(tmp_str, str, size);
	tmp_str[size] = 0;
	uint16_t tmp[sizeof(ipv6_int)/2];
	if (inet_pton(AF_INET6, tmp_str, &tmp[0]) == 0) {
		valid = false;
		return 0;
	}
	valid = true;
	ipv6_int ret;
	uint16_t* buf = (uint16_t*) &ret;
	for (size_t i = 0; i < sizeof(ret)/2; i++) {
		buf[sizeof(ret)/2-i-1] = ntohs(tmp[i]);
	}
	free(tmp_str);
	return ret;
}

leeloo::ipv6_int leeloo::ips_parser::ipv6toi(const char* str, bool& valid)
{
	return ipv6toi(str, strlen(str), valid);
}

template <bool exclude>
static bool __parse_ipv6s(leeloo::ipv6_list_intervals& l, const char* str)
{
	// We want to support these formats:
	// 2001:0DB8::/32
	// 2001:0DB8:: - 2001:0DB9::
	// 2001:0DB8:AA-BBCC:1::1
	// 2001:0DB8:*:*:*:*:*:1
	
	const size_t size_str = strlen(str);

	// TODO: merge these loops
	const size_t ndashes = count_char_buf(str, size_str, '-');
	const size_t nslashes = count_char_buf(str, size_str, '/');
	const size_t nstars = count_char_buf(str, size_str, '*');

	// Fast case with no slashes or dashes (simple IP)
	if ((nslashes == 0) && (ndashes == 0) && (nstars == 0)) {
		// Just parse the IP
		bool valid;
		const leeloo::ipv6_int ip = leeloo::ips_parser::ipv6toi(str, size_str, valid);
		if (!valid) {
			return false;
		}

		l.insert<exclude>(ip, ip);
		return true;
	}

	if (ndashes == 1) {
		if ((nslashes > 0) || (nstars > 0)) {
			return false;
		}

		// We want to parse this:
		// 2001:ABCD::1 - 2002:ABCD::1
		// We support the fact that the interval could have been written the wrong way.
		const char* sep = strchr(str, '-');
		if (*(sep+1) == 0) {
			return false;
		}
		const size_t idx_sep = (uintptr_t)sep-(uintptr_t)str;

		bool valid = false;
		leeloo::ipv6_int a = leeloo::ips_parser::ipv6toi(str, idx_sep, valid);
		if (!valid) {
			return false;
		}
		const size_t size_part2 = size_str-(idx_sep+1);
		leeloo::ipv6_int b = leeloo::ips_parser::ipv6toi(sep+1, size_part2, valid);
		if (!valid) {
			// Check if this is just a 16bit hexadecimal number
			char* end;
			int b_ = strtol(sep+1, &end, 16);
			if (end == (sep+1) || (b_ < 0) || (b_ > 0xFFFF)) {
				return false;
			}
			b = (a & (~leeloo::ipv6_int(0xFFFF))) | ((unsigned short)b_);
		}
		if (a > b) {
			std::swap(a, b);
		}
		l.insert<exclude>(a, b);
		return true;
	}

	if (nslashes > 1) {
		return false;
	}

	if ((ndashes == 0) && (nstars == 0)) {
		const char* slash = strchr(str, '/');
		if (!slash || strlen(slash) == 0) {
			return false;
		}
		const int prefix = strtol(slash+1, nullptr, 10);
		if (prefix < 0) {
			return false;
		}
		bool valid = false;
		leeloo::ipv6_int ip_start = leeloo::ips_parser::ipv6toi(str, (uintptr_t)slash-(uintptr_t)str, valid);
		if (!valid) {
			return false;
		}

		l.insert_prefix<exclude>(ip_start, prefix);

		return true;
	}

	short_interval intervals[8];
	const char* cur = str;
	int cur_interval = 0;
	const char* colon;
	bool seen_double = false;
	while (((colon = strchr(cur, ':')) != nullptr) && cur_interval < 8) {
		if (*(colon + 1) == ':') {
			if (seen_double) {
				// Can't have twice '::'
				return false;
			}
			seen_double = true;
			size_t ncolons = count_char_buf(str, size_str, ':');
			const int interval_end = (8-ncolons)+cur_interval;
			for (; cur_interval <= interval_end; cur_interval++) {
				intervals[cur_interval].set(0, 0);
			}
			cur = colon+2;
			continue;
		}

		const char* dash = (const char*) memchr(cur, '-', (uintptr_t)colon-(uintptr_t)cur);
		if (dash) {
			char* end;
			const int32_t short_min = strtol(cur, &end, 16);
			if (end == cur) {
			   return false;
			}
			const int32_t short_max = strtol(dash+1, &end, 16);
			if (end == (dash+1)) {
				return false;
			}
			if ((short_min < 0) || (short_min > 0xFFFF) || (short_max < 0) || (short_max > 0xFFFF)) {
				return false;
			}
			intervals[cur_interval].set(short_min, short_max);
		}
		else {
			char* end;
			const int32_t s = strtol(cur, &end, 16);
			if (end == cur) {
				return false;
			}
			if ((s < 0) || (s > 0xFFFF)) {
				return false;
			}
			intervals[cur_interval].set(s, s);
		}
		cur = colon+1;
		cur_interval++;
	}

	// Check that 7 intervals have been found
	if (cur_interval != 7) {
		return false;
	}

	if (seen_double && (*(cur-1)) == ':') {
		// It was ended by a double colon. We miss a short interval!
		intervals[cur_interval].set(0, 0);
	}
	else {
		// Process the last one
		const char* dash = (const char*) memchr(cur, '-', (uintptr_t)colon-(uintptr_t)cur);
		if (dash) {
			char* end;
			const int32_t short_min = strtol(cur, &end, 16);
			if (end == cur) {
			   return false;
			}
			const int32_t short_max = strtol(dash+1, &end, 16);
			if (end == (dash+1)) {
				return false;
			}
			if ((short_min < 0) || (short_min > 0xFFFF) || (short_max < 0) || (short_max > 0xFFFF)) {
				return false;
			}
			intervals[cur_interval].set(short_min, short_max);
		}
		else {
			char* end;
			const int32_t s = strtol(cur, &end, 16);
			if (end == cur) {
				return false;
			}
			if ((s < 0) || (s > 0xFFFF)) {
				return false;
			}
			intervals[cur_interval].set(s, s);
		}
	}

	for (int i = 0; i < 8; i++) {
		std::cerr << std::hex << intervals[i].min << "\t" << intervals[i].max << std::endl;
	}

	return false;
}

bool leeloo::ips_parser::parse_ipv6s_add(ipv6_list_intervals& l, const char* str)
{
	return __parse_ipv6s<false>(l, str);
}

bool leeloo::ips_parser::parse_ipv6s_remove(ipv6_list_intervals& l, const char* str)
{
	return __parse_ipv6s<true>(l, str);
}

#endif
