/* 
 * Copyright (c) 2013, Quarkslab
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

#include <leeloo/ips_parser.h>
#include <leeloo/ip_list_intervals.h>

// This is a closed interval [min, max]
struct byte_interval
{
	void set(uint8_t const min_, uint8_t const max_)
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

	uint8_t min;
	uint8_t max;
};

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

static inline uint32_t cidr2size(const int cidr)
{
	assert(cidr > 0);
	return 1U<<(32-cidr);
}

static inline uint32_t cidr2mask(const int cidr)
{
	assert(cidr > 0);
	return (1U<<(32-cidr))-1;
}

uint32_t leeloo::ips_parser::ipv4toi(const char* str, const size_t size, bool& valid, int min_dots)
{
	if (size == 0) {
		valid = false;
		return 0;
	}

	uint32_t ret = 0;
	int cur_idx = 3;
	size_t start_idx = 0;
	for (size_t i = 0; i <= size; i++) {
		const char c = str[i];
		if (c == '.' || (i == size)) {
			if (cur_idx < 0) {
				valid = false;
				return 0;
			}

			const uint32_t cur_int = atoi3(&str[start_idx], i-start_idx);
			if (cur_int > 0xFF) {
				valid = false;
				return 0;
			}
			ret |= cur_int << (8*cur_idx);
			cur_idx--;
			start_idx = i+1;
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
		uint32_t b = leeloo::ips_parser::ipv4toi(sep+1, size_str-(idx_sep+1), valid);
		if (!valid) {
			return false;
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
		// Do not accept '/0'...
		if (cidr <= 0) {
			return false;
		}
		bool valid = false;
		uint32_t ip_start = leeloo::ips_parser::ipv4toi(str, (uintptr_t)slash-(uintptr_t)str, valid, 0);
		if (!valid) {
			return false;
		}

		const uint32_t mask = cidr2mask(cidr);
		l.insert<exclude>(ip_start & (~mask), ip_start|mask);

		return true;
	}

	// Parse recursively block by block (separated by '.')
	byte_interval intervals[4];
	const char* cur = str;
	int cur_interval = 0;
	const char* dot;
	while (((dot = strchr(cur, '.')) != nullptr) && cur_interval < 4) {
		//*dot = 0;
		/*char* slash = strchr(cur, '/');
		if (slash) {
			// End of loop
		}*/
		const char* dash = (const char*) memchr(cur, '-', (uintptr_t)dot-(uintptr_t)cur);
		if (dash) {
			const int32_t byte_min = atoi3(cur, (uintptr_t)dash-(uintptr_t)cur);
			const int32_t byte_max = atoi3(dash+1, (uintptr_t)dot-(uintptr_t)(dash+1));
			if (byte_min == -1 || byte_max == -1) {
				return false;
			}
			intervals[cur_interval].set(byte_min, byte_max);
		}
		else {
			const int32_t byte = atoi3(cur, (uintptr_t)dot-(uintptr_t)cur);
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
		const int32_t byte_min = atoi3(cur, (uintptr_t)dash-(uintptr_t)cur);
		const int32_t byte_max = atoi3(dash+1, size_str-((uintptr_t)(dash+1)-(uintptr_t)str));
		if (byte_min == -1 || byte_max == -1) {
			return false;
		}
		intervals[cur_interval].set(byte_min, byte_max);
	}
	else {
		const int32_t byte = atoi3(cur, (uintptr_t)dot-(uintptr_t)cur);
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
