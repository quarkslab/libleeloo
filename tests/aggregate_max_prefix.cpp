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

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <set>
#include <random>

#include <leeloo/interval.h>
#include <leeloo/ip_list_intervals.h>

template <class Interval>
void print_intervals(Interval const& l)
{
	typedef typename Interval::interval_type interval_type;
	for (interval_type const& i: l.intervals()) {
		printf("0x%x - 0x%x\n", i.lower(), i.upper());
	}
}

template <class Interval>
int compare_intervals(Interval const& l, typename Interval::base_type const* const ref, size_t const ninter)
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
	leeloo::ip_list_intervals list;
	list.add("10.0.0.4-10.0.0.25");
	list.add("10.0.0.46-10.0.0.125");
	list.add("10.0.0.76-10.0.1.4");
	list.add("10.0.2.0-10.0.4.4");
	list.add("10.0.7.0-10.0.7.4");

	list.aggregate_max_prefix(24);
	print_intervals(list);

#define COMPARE()\
	int ret_ = compare_intervals(list, intervals_agg, sizeof(intervals_agg)/(2*sizeof(uint32_t)));\
	if (ret_ != 0) {\
		std::cerr << "error" << std::endl;\
		ret = ret_;\
	}

	{
		uint32_t intervals_agg[] = {
			0x0a000000, 0x0a000500, // 10.0.0.0->10.0.4.255
			0x0a000700, 0x0a000800, // 10.0.7.0/24

		};
		COMPARE()
	}

	list.clear();
	list.add("10.0.0.4-10.0.0.25");
	list.add("10.0.0.46-10.0.0.125");
	list.add("10.0.0.76-10.0.1.4");
	list.add("10.0.2.0-10.0.4.4");
	list.add("10.0.7.0-10.0.7.4");

	list.aggregate_max_prefix_strict(24);
	print_intervals(list);

	{
		uint32_t intervals_agg[] = {
			0x0a000000, 0x0a000405, // 10.0.0.0->10.0.4.5
			0x0a000700, 0x0a000800, // 10.0.7.0/24
		};
		COMPARE()
	}

	list.clear();
	list.add("10.0.1.0/24");
	list.add("10.0.5.0/24");

	list.aggregate_max_prefix(30);
	print_intervals(list);
	{
		uint32_t intervals_agg[] = {
			0x0a000100, 0x0a000200, // 10.0.1.0/24
			0x0a000500, 0x0a000600, // 10.0.5.0/24
		};
		COMPARE()
	}

	list.clear();
	list.add("10.0.1.0/24");
	list.add("10.0.5.0/24");

	list.aggregate_max_prefix_strict(30);
	print_intervals(list);
	{
		uint32_t intervals_agg[] = {
			0x0a000100, 0x0a000200, // 10.0.1.0/24
			0x0a000500, 0x0a000600, // 10.0.5.0/24
		};
		COMPARE()
	}

	return ret;
}
