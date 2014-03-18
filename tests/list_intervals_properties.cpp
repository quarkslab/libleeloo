/* 
 * Copyright (c) 2014, Quarkslab
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

#include <boost/random.hpp>

#include <leeloo/interval.h>
#include <leeloo/list_intervals_properties.h>

template <class Interval>
void print_intervals(Interval const& l)
{
	typedef typename Interval::interval_type interval_type;
	for (interval_type const& i: l.intervals()) {
		std::cout << i.lower() << " " << i.upper() << std::endl;
	}
}

typedef std::vector<int> property;
typedef leeloo::interval<uint32_t> interval;

// Interval of type [a,b[
typedef leeloo::list_intervals_properties<interval, property, uint32_t> list_intervals_properties;

void print_property(property const& p)
{
	for (int i: p) {
		std::cout << i << ",";
	}
	std::cout << std::endl;
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " n" << std::endl;
		return 1;
	}

	list_intervals_properties list;
	list.add(0, 2);
	list.add(5, 9);
	list.add(1, 5);
	list.add(8, 9);
	list.add(9, 15);
	list.add(19, 21);
	list.add_property(interval(5, 11), {1});
	list.add_property(interval(9, 14), {2});
	list.add_property(interval(12, 16), {4});
	list.add_property(interval(16, 20), {5});
	list.add_property(interval(1, 20), {6});
	list.aggregate();
	list.aggregate_properties([](property& org, property const& o)
			{
				for (int i: o) {
					org.push_back(i);
				}
			});

	print_intervals(list);
	property const* p;
	p = list.property_of(5);
	print_property(*p);
	p = list.property_of(8);
	print_property(*p);
	p = list.property_of(9);
	print_property(*p);
	p = list.property_of(10);
	print_property(*p);

	return 0;
}
