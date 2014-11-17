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

#include <leeloo/ipv6_list_intervals.h>
#include <leeloo/ips_parser.h>

#include <iostream>
#include <iomanip>

#include <string.h>

void print_intervals(leeloo::ipv6_list_intervals const& l)
{
	for (auto const& i: l.intervals()) {
		std::cout << std::hex << std::setw(16) << std::setfill('0') << i.lower() << "\t" << i.upper() << std::endl;
	}
}

leeloo::ipv6_int make_ipv6(uint64_t const high, uint64_t const low)
{
	leeloo::ipv6_int ret = low;
	leeloo::ipv6_int high_ = high;
	ret |= high_<<64;
	return ret;
}

int main()
{
	bool valid;
	leeloo::ipv6_int tmp;
	int ret = 0;

	std::cerr << std::hex;

#define TEST_IPV6I(str, res, valid_)\
	tmp = leeloo::ips_parser::ipv6toi(str, valid);\
	std::cout << "Test with " << str << "\t" << valid << "\t" << std::hex << tmp << std::endl;\
	if (valid != valid_) {\
		std::cerr << "Wrongly guessed whether it was valid or not for '" << str << "'" << std::endl;\
		ret = 1;\
	}\
	else\
	if (valid && (tmp != res)) {\
		std::cerr << "Results is invalid for '" << str << "': was expecting " << res << ", got " << tmp << " instead." << std::endl;\
		ret = 1;\
	}

#define TEST_IP_PARSER(str, is_valid, ip_start, ip_end)\
	{\
		std::cout << "Test with " << str << std::endl;\
		leeloo::ipv6_list_intervals out;\
		if (leeloo::ips_parser::parse_ipv6s(out, str) != is_valid) {\
			std::cerr << "Return code of parse_ips isn't valid for '" << str << "'!" << std::endl;\
			ret = 1;\
		}\
		out.aggregate();\
		if (is_valid) {\
			leeloo::ipv6_list_intervals ref;\
			ref.add(leeloo::ips_parser::ipv6toi(ip_start, valid), leeloo::ips_parser::ipv6toi(ip_end, valid));\
			if (ref != out) {\
				std::cerr << "Interval computed is bad for " << str << "!" << std::endl;\
				std::cerr << "got:" << std::endl;\
				print_intervals(out);\
				std::cerr << "expected:" << std::endl;\
				print_intervals(ref);\
				ret = 1;\
			}\
		}\
	}

	std::cout << "TEST_IPV6I ipv6toi..." << std::endl;
	TEST_IPV6I(" ", 0, false);
	TEST_IPV6I("      ", 0, false);
	TEST_IPV6I("  1   ", 0, false);
	TEST_IPV6I("2001::", make_ipv6(0x2001000000000000ULL, 0), true);
	TEST_IPV6I("google.com", 0, false);

	std::cout << "Test IPv6 parser..." << std::endl;
	TEST_IP_PARSER("2001:0DB8::/32", true, "2001:0DB8::", "2001:0DB8:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");

	return ret;
}
