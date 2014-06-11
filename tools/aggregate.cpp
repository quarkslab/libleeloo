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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <x86intrin.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <memory>

#include <leeloo/ip_list_intervals.h>

static inline const char* ip2str(uint32_t ip)
{
	ip = htonl(ip);
	return inet_ntoa(*reinterpret_cast<struct in_addr const*>(&ip));
}

static void display(leeloo::ip_interval const& it)
{
	const uint32_t width = it.width();
	const char* ip_a = ip2str(it.lower());
	if (__builtin_popcount(width) == 1) {
		// CIDR prefix
		const int prefix = 32-__builtin_ctz(width);
		std::cout << ip_a << "/" << prefix << std::endl;
	}
	else {
		// AG: as a static buffer is used for inet_ntoa, we need to make two
		// std::cout statements here, or the same IP will be shown twice!
		std::cout << ip_a << "-"; 
		std::cout << ip2str(it.upper()) << std::endl;
	}
}

static void usage(const char* path)
{
	std::cerr << "Usage: " << path << " [--help] [--max-prefix max_prefix] [input_file]" << std::endl;
	std::cerr << "where:\n" << std::endl;
	std::cerr << "\t--help: show this help" << std::endl;
	std::cerr << "\t--max-prefix: aggregate with a maximum prefix (1 <= prefix <= 32)" << std::endl;
	std::cerr << "\t[input_file] is a list of IPs ranges (defaults to stdin)\n" << std::endl;

	std::cerr << "IP ranges can be defined as:\n" << std::endl;
	std::cerr << "\tSingle IP:     192.168.0.1" << std::endl;
	std::cerr << "\tCIDR notation: 192.168.4.0/24" << std::endl;
	std::cerr << "\tIP interval:   192.168.4.0-192.168.6.0" << std::endl;
	std::cerr << "\t               192.168-170.4-8.0" << std::endl;
}

int main(int argc, char** argv)
{
	std::istream* in = &std::cin; 
	std::unique_ptr<std::ifstream> in_storage;
	int max_prefix = -1;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--max-prefix") == 0) {
			i++;
			if (i >= argc) {
				break;
			}
			max_prefix = atoi(argv[i]);
			if (max_prefix <= 0 || max_prefix > 32) {
				std::cerr << "Maximum prefix must be between 1 and 32." << std::endl;
				return 1;
			}
		}
		else
		if (strcmp(argv[i], "--help") == 0) {
			usage(argv[0]);
			return 1;
		}
		else {
			if (in != &std::cin) {
				usage(argv[0]);
				return 1;
			}
			const char* file = argv[i];
			std::unique_ptr<std::ifstream> f(new std::ifstream(file, std::ifstream::in));
			if (!(*f)) {
				std::cerr << "Error opening " << file << ": " << strerror(errno) << std::endl;
				return errno;
			}
			in = f.get();
			in_storage = std::move(f);
		}
	}

	leeloo::ip_list_intervals l;

	std::string line;
	while (std::getline(*in, line)) {
		if (!l.add(line.c_str())) {
			std::cerr << "Warning: unable to parse '" << line << "'. Ignoring..." << std::endl;
		}
	}

	if (max_prefix == -1) {
		l.aggregate();
	}
	else {
		l.aggregate_max_prefix(max_prefix);
	}

	for (leeloo::ip_interval const& it: l) {
		display(it);
	}

	return 0;
}
