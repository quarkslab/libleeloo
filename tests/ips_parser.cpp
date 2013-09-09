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

int main()
{
	bool valid;
	uint32_t ret;

#define TEST_IPV4I(str, res)\
	ret = leeloo::ips_parser::ipv4toi(str, strlen(str), valid);\
	std::cout << str << "\t" << valid << "\t" << std::hex << ret << std::endl;\
	if (ret != res) {\
		std::cerr << "Results is invalid: " << res << " expected." << std::endl;\
		return 1;\
	}

#define TEST_IP_PARSER(str, is_valid, ip_start, ip_end)\
	{\
		std::cout << str << std::endl;\
		leeloo::ip_list_intervals out;\
		if (leeloo::ips_parser::parse_ips(out, str) != is_valid) {\
			std::cerr << "Return code of parse_ips isn't valid!" << std::cerr;\
			return 1;\
		}\
		if (is_valid) {\
			leeloo::ip_list_intervals ref;\
			ref.add(leeloo::ips_parser::ipv4toi(ip_start, strlen(ip_start), valid), leeloo::ips_parser::ipv4toi(ip_end, strlen(ip_end), valid));\
			if (ref != out) {\
				std::cerr << "Interval computed is bad!" << std::endl;\
				return 1;\
			}\
		}\
	}

	std::cout << "TEST_IPV4I ipv4toi..." << std::endl;
	TEST_IPV4I("0.0.0.10", 0x0a);
	TEST_IPV4I("0.0.10.0", 0x0a00);
	TEST_IPV4I("0.10.0.0", 0x0a0000);
	TEST_IPV4I("10.0.0.0", 0x0a000000);
	TEST_IPV4I("10.10.10.10", 0x0a0a0a0a);
	TEST_IPV4I("111.111.111.111", 0x6f6f6f6f);
	TEST_IPV4I("1.11.111.1", 0x010b6f01);
	TEST_IPV4I("255.255.255.255", 0xffffffff);
	TEST_IPV4I("255.255..255", 0);
	TEST_IPV4I("0.0.0.0", 0);
	TEST_IPV4I("0", 0);
	TEST_IPV4I("0.0.0.", 0);
	TEST_IPV4I("0.0.0", 0);
	TEST_IPV4I("0.1240.0.10", 0);
	TEST_IPV4I("...", 0);
	TEST_IPV4I("12.124.5.678", 0);
	TEST_IPV4I("-12.124.5.678", 0);

	ret = leeloo::ips_parser::ipv4toi("10", 2, valid, 0);
	std::cout << std::hex;
	std::cout << "10" << "\t" << valid << "\t" << ret << std::endl;


	std::cout << "Test ips parser..." << std::endl;
	TEST_IP_PARSER("10.0.0.0-10.0.0.255", true, "10.0.0.0", "10.0.0.255");
	TEST_IP_PARSER("127.0.0.1", true, "127.0.0.1", "127.0.0.1");
	TEST_IP_PARSER("blabla", false, "", "");
	TEST_IP_PARSER("google.com", false, "", "");
	TEST_IP_PARSER("10/8", true, "10.0.0.0", "10.255.255.255");
	TEST_IP_PARSER("192.168.10.1/24", true, "192.168.10.0", "192.168.10.255");
	TEST_IP_PARSER("192.168.10.1/16", true, "192.168.0.0", "192.168.255.255");
	TEST_IP_PARSER("10/2", true, "0.0.0.0", "63.255.255.255");
	{
		std::cout << "10-10.1-1.5-5.20-19" << std::endl;
		leeloo::ip_list_intervals out;
		leeloo::ips_parser::parse_ips(out, "10-10.1-1.5-5.20-19");
		print_intervals(out);
	}
	{
		std::cout << "10-15.1-1.5-9.20-25" << std::endl;
		leeloo::ip_list_intervals out;
		leeloo::ips_parser::parse_ips(out, "10-15.1-4.5-9.20-25");
		print_intervals(out);
	}
	{
		std::cout << "10--10.1.5.20----25" << std::endl;
		leeloo::ip_list_intervals out;
		if (leeloo::ips_parser::parse_ips(out, "10--10.1.5.20----25")) {
			std::cerr << "Parsing worked, shouldn't have!" << std::endl;
			return 1;
		}
	}

	return 0;
}
