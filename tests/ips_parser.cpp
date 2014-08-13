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

#include <leeloo/ip_list_intervals.h>
#include <leeloo/ips_parser.h>

#include <iostream>
#include <iomanip>

#include <string.h>

void print_intervals(leeloo::ip_list_intervals const& l)
{
	for (auto const& i: l.intervals()) {
		std::cout << std::hex << std::setw(8) << std::setfill('0') << i.lower() << "\t" << i.upper() << std::endl;
	}
}

int test_dashes(const char* str, leeloo::ip_list_intervals const& ref)
{
	std::cout << "Test with " << str << std::endl;

	leeloo::ip_list_intervals out;
	leeloo::ips_parser::parse_ips(out, str);
	if (out != ref) {
		std::cerr << "Error parsing " << str << "..." << std::endl;
		std::cerr << "got:" << std::endl;
		print_intervals(out);
		std::cerr << "expected:" << std::endl;
		print_intervals(ref);
		return 1;
	}
	return 0;
}

int main()
{
	bool valid;
	uint32_t ret = 0;

	std::cerr << std::hex;

#define TEST_IPV4I(str, res, valid_)\
	ret = leeloo::ips_parser::ipv4toi(str, valid);\
	std::cout << "Test with " << str << "\t" << valid << "\t" << std::hex << ret << std::endl;\
	if (valid != valid_) {\
		std::cerr << "Wrongly guessed whether it was valid or not for " << str << std::endl;\
		ret = 1;\
	}\
	else\
	if (valid && (ret != res)) {\
		std::cerr << "Results is invalid for " << str << ": was expecting " << res << ", got " << ret << " instead." << std::endl;\
		ret = 1;\
	}

#define TEST_IP_PARSER(str, is_valid, ip_start, ip_end)\
	{\
		std::cout << "Test with " << str << std::endl;\
		leeloo::ip_list_intervals out;\
		if (leeloo::ips_parser::parse_ips(out, str) != is_valid) {\
			std::cerr << "Return code of parse_ips isn't valid for " << str << "!" << std::cerr;\
			ret = 1;\
		}\
		out.aggregate();\
		if (is_valid) {\
			leeloo::ip_list_intervals ref;\
			ref.add(leeloo::ips_parser::ipv4toi(ip_start, strlen(ip_start), valid), leeloo::ips_parser::ipv4toi(ip_end, strlen(ip_end), valid));\
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

	std::cout << "TEST_IPV4I ipv4toi..." << std::endl;
	TEST_IPV4I("0.0.0.10", 0x0a, true);
	TEST_IPV4I("0.0.10.0", 0x0a00, true);
	TEST_IPV4I("0.10.0.0", 0x0a0000, true);
	TEST_IPV4I("10.0.0.0", 0x0a000000, true);
	TEST_IPV4I("10.10.10.10", 0x0a0a0a0a, true);
	TEST_IPV4I("111.111.111.111", 0x6f6f6f6f, true);
	TEST_IPV4I("1.11.111.1", 0x010b6f01, true);
	TEST_IPV4I("255.255.255.255", 0xffffffff, true);
	TEST_IPV4I("255.255..255", 0, false);
	TEST_IPV4I("0.0.0.0", 0, true);
	TEST_IPV4I("0", 0, false);
	TEST_IPV4I("0.0.0.", 0, false);
	TEST_IPV4I("0.0.0", 0, false);
	TEST_IPV4I("0.1240.0.10", 0, false);
	TEST_IPV4I("...", 0, false);
	TEST_IPV4I("12.124.5.678", 0, false);
	TEST_IPV4I("-12.124.5.678", 0, false);
	TEST_IPV4I("10", 0, false);
	TEST_IPV4I("google.com", 0, false);

	std::cout << "Test ips parser..." << std::endl;
	TEST_IP_PARSER("10.0.0.0-10.0.0.255", true, "10.0.0.0", "10.0.0.255");
	TEST_IP_PARSER("127.0.0.1", true, "127.0.0.1", "127.0.0.1");
	TEST_IP_PARSER("blabla", false, "", "");
	TEST_IP_PARSER("google.com", false, "", "");
	TEST_IP_PARSER("10/8", true, "10.0.0.0", "10.255.255.255");
	TEST_IP_PARSER("192.168.10.1/24", true, "192.168.10.0", "192.168.10.255");
	TEST_IP_PARSER("192.168.10.1/16", true, "192.168.0.0", "192.168.255.255");
	TEST_IP_PARSER("10/2", true, "0.0.0.0", "63.255.255.255");
	TEST_IP_PARSER("10--10.1.5.20----25", false, "", "");
	{
		leeloo::ip_list_intervals ref;
		ref.add(leeloo::ips_parser::ipv4toi("10.1.5.19", valid), leeloo::ips_parser::ipv4toi("10.1.5.20", valid));
		ret |= test_dashes("10-10.1-1.5-5.20-19", ref);
	}
	{
		leeloo::ip_list_intervals ref;
		for (uint32_t a = 10; a <= 15; a++) {
			for (uint32_t b = 1; b <= 4; b++) {
				for (uint32_t c = 5; c <= 9; c++) {
					const uint32_t ip_a = (a<<24) | (b<<16) | (c<<8) | 20;
					const uint32_t ip_b = (a<<24) | (b<<16) | (c<<8) | 25;
					ref.add(ip_a, ip_b);
				}
			}
		}

		ret |= test_dashes("10-15.1-4.5-9.20-25", ref);
	}

	{
		leeloo::ip_list_intervals ref;
		ref.add(leeloo::ips_parser::ipv4toi("10.4.5.8", valid), leeloo::ips_parser::ipv4toi("10.4.5.20", valid));
		ret |= test_dashes("10.4.5.8-20", ref);
	}

	return ret;
}
