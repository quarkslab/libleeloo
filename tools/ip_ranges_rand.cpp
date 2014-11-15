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

#include <iostream>

#include <random>

#include <leeloo/ip_list_intervals.h>
#include <leeloo/ips_parser.h>
#include <leeloo/random.h>

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " range1 [-][range2] [-][range3] ..." << std::endl << std::endl;
		std::cerr << "This tool will aggregate all the given ranges and will write a random order of the corresponding IP." << std::endl << std::endl;
		std::cerr << "where range can be described as:" << std::endl;
		std::cerr << "\tCIDR notation:\t192.168.1.0/24" << std::endl;
		std::cerr << "\tRanges:\t\t10.4-5.8.9-250" << std::endl;
		std::cerr << "\tSingle IP:\t192.168.1.10" << std::endl << std::endl;
		std::cerr << "A '-' symbol before any range will remove it from the final set." << std::endl;
		return 1;
	}

	leeloo::ip_list_intervals l;
	for (int i = 1; i < argc; i++) {
		const char* const ip_range = argv[i];
		bool ret;
		if (strlen(ip_range) <= 1) {
			ret = false;
		}
		else
		if (ip_range[0] == '-') {
			ret = l.remove(&ip_range[1]);
		}
		else {
			ret = l.add(ip_range);
		}

		if (!ret) {
			std::cerr << "Warning: '" << ip_range << "' is an invalid IP range." << std::endl;
		}
	}

	// Aggregate all IPs
	l.aggregate();

	std::cerr << "Number of IPs after aggregation: " << l.size() << std::endl;

	l.create_index_cache(256);

	// Initialize a random generator
	std::random_device rd;

	l.random_sets(16, 
		[](const uint32_t* ips, const size_t n)
		{
			for (size_t i = 0; i < n; i++) {
				std::cout << leeloo::ips_parser::ipv4tostr(ips[i]) << std::endl;
			}
		},
		rd);

	return 0;
}
