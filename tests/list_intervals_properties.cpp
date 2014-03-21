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
#include <leeloo/list_intervals_with_properties.h>
#include <leeloo/random.h>

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

void print_property(int v, property const& p)
{
	std::cout << v << ": ";
	for (int i: p) {
		std::cout << i << ",";
	}
	std::cout << std::endl;
}

int main(int argc, char** argv)
{
	srand(time(NULL));

	list_intervals_properties list;
	list.add_property(interval(5, 11), {1});
	list.add_property(interval(9, 14), {2});
	list.add_property(interval(12, 16), {4});
	list.add_property(interval(16, 20), {5});
	list.add_property(interval(1, 20), {6});
	list.aggregate_properties(
			[](property& org, property const& o)
			{
				for (int i: o) {
					org.push_back(i);
				}
			},
			[](property& org, property const& o)
			{
				property::iterator it;
				for (int i: o) {
					it = std::find(org.begin(), org.end(), i);
					if (it != org.end()) {
						org.erase(it);
					}
				}
			});

	property const* p;
	p = list.property_of(1);
	print_property(1, *p);
	p = list.property_of(5);
	print_property(5, *p);
	p = list.property_of(6);
	print_property(6, *p);
	p = list.property_of(9);
	print_property(9, *p);
	p = list.property_of(12);
	print_property(12, *p);
	p = list.property_of(15);
	print_property(15, *p);
	p = list.property_of(16);
	print_property(16, *p);
	p = list.property_of(18);
	print_property(18, *p);

	list_intervals_properties list2;
	list2.add_property(interval(5, 11), {1});
	list2.add_property(interval(9, 14), {2});
	list2.add_property(interval(12, 16), {4});
	list2.add_property(interval(16, 20), {5});
	list2.add_property(interval(1, 20), {6});
	list2.aggregate_properties_no_rem(
			[](property& org, property const& o)
			{
				for (int i: o) {
					org.push_back(i);
				}
			});
	for (size_t i = 0; i < 20; i++) {
		property const* ref = list.property_of(i);
		property const* cmp = list2.property_of(i);
		if (ref == nullptr) {
			if (cmp != nullptr) {
				std::cerr << "error with " << i << std::endl;
			}
			continue;
		}
		else
		if (cmp == nullptr) {
			std::cerr << "error with " << i << std::endl;
			continue;
		}
		if (*ref != *cmp) {
			std::cerr << "error with " << i << std::endl;
		}
	}

	leeloo::list_intervals_with_properties<leeloo::list_intervals<interval>, property> lip;
	lip.add(0, 2);
	lip.add(5, 9);
	lip.add(1, 5);
	lip.add(8, 9);
	lip.add(9, 15);
	lip.add(19, 21);
	lip.add_property(interval(5, 11), {1});
	lip.add_property(interval(9, 14), {2});
	lip.add_property(interval(12, 16), {4});
	lip.add_property(interval(16, 20), {5});
	lip.add_property(interval(1, 20), {6});
	lip.aggregate();
	lip.aggregate_properties(
			[](property& org, property const& o)
			{
				for (int i: o) {
					org.push_back(i);
				}
			},
			[](property& org, property const& o)
			{
				property::iterator it;
				for (int i: o) {
					it = std::find(org.begin(), org.end(), i);
					if (it != org.end()) {
						org.erase(it);
					}
				}
			});
	lip.create_index_cache(16);

	std::cout << "random_sets_with_properties" << std::endl;

	boost::random::mt19937 mt_rand(time(NULL));
	lip.random_sets_with_properties(4,
		[](uint32_t const* ints, property const* const* properties, size_t n)
		{
			for (size_t i = 0; i < n; i++) {
				std::cout << ints[i] << ": ";
				property const* p = properties[i];
				if (p == nullptr) {
					std::cout << "no properties";
				}
				else {
					for (int i: *p) {
						std::cout << i << ",";
					}
				}
				std::cout << std::endl;
			}
		},
		leeloo::random_engine<uint32_t>(mt_rand));

	return 0;
}
