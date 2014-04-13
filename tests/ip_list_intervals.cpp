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
#include <iostream>

int compare_intervals(leeloo::ip_list_intervals const& l, uint32_t const* const ref, size_t const ninter)
{
	if (l.intervals().size() != ninter) {
		std::cerr << "bad number of intervals" << std::endl;
		return 1;
	}
	int ret = 0;
	size_t i = 0;
	for (auto const& it: l.intervals()) {
		if ((it.lower() != ref[2*i]) ||
			(it.upper() != ref[2*i+1])) {
			std::cerr << "invalid interval at index " << i << std::endl;
			ret = 1;
		}
		i++;
	}
	return ret;
}

int main()
{
	int ret = 0;

	leeloo::ip_list_intervals l;
	l.add("10.1-2.4-5.6-7");
	l.add("192.168.0.0/24");
	l.add("10.0.0.1");
	l.add("10.0.0.2");
	l.add("192.168.0.1");
	l.add("172.16.0/24");
	l.add("10.1.1.1");

	uint32_t intervals_before[] = {
		0xa010406, 0xa010408,   // 10.1.4.6-7
		0xa010506, 0xa010508,   // 1O.1.5.6-7
		0xa020406, 0xa020408,   // 10.2.4.6-7
		0xa020506, 0xa020508,   // 10.2.5.6-7
		0xc0a80000, 0xc0a80100, // 192.168.0.0/24
		0xa000001, 0xa000002,   // 10.0.0.1
		0xa000002, 0xa000003,   // 10.0.0.2
		0xc0a80001, 0xc0a80002, // 192.168.0.1
		0xac100000, 0xac100100, // 172.16.0/24
		0xa010101, 0xa010102    // 10.1.1.1
	};

	ret = compare_intervals(l, intervals_before, sizeof(intervals_before)/(2*sizeof(uint32_t)));

	l.aggregate();

	uint32_t intervals_after[] = {
		0xa000001, 0xa000003,
		0xa010101, 0xa010102,
		0xa010406, 0xa010408,
		0xa010506, 0xa010508,
		0xa020406, 0xa020408,
		0xa020506, 0xa020508,
		0xac100000, 0xac100100,
		0xc0a80000, 0xc0a80100
	};

	ret = compare_intervals(l, intervals_after, sizeof(intervals_after)/(2*sizeof(uint32_t)));

	return ret;
}
