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
#include <array>

#include <boost/random.hpp>

#include <leeloo/interval.h>
#include <leeloo/list_intervals_properties.h>
#include <leeloo/list_intervals_with_properties.h>
#include <leeloo/random.h>

#include <random>

template <class Interval>
void print_intervals(Interval const& l)
{
	typedef typename Interval::interval_type interval_type;
	for (interval_type const& i: l.intervals()) {
		std::cout << i.lower() << " " << i.upper() << std::endl;
	}
}

typedef std::vector<int64_t> property;
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


template <size_t N, class O, class T>
bool compare(O const& o, std::array<T, N> const& ref) 
{
    if (o.size() != N) {
        return false;
    }   
    size_t i = 0;
    for (T const& v: o) {
        if (v != ref[i]) {
            return false;
        }   
        i++;
    }   
    return true;

}

template <size_t N>
int check_property(list_intervals_properties const& list, size_t idx, std::array<int, N> const& v)
{
	property const* p;
	p = list.property_of(idx);
	if (!compare<N>(*p, v)) {
		std::cerr << "Error, expected:" << std::endl; 
		print_property(idx, *p);
		return 1;
	}
	return 0;
}


int main()
{
	int ret = 0;
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

	ret = check_property(list, 1, std::array<int, 1>{{6}}); // double braces makes compiler happy!
	ret = check_property(list, 5, std::array<int, 2>{{6, 1}});
	ret = check_property(list, 6, std::array<int, 2>{{6, 1}});
	ret = check_property(list, 9, std::array<int, 3>{{6, 1, 2}});
	ret = check_property(list, 12, std::array<int, 3>{{6, 2, 4}});
	ret = check_property(list, 15, std::array<int, 2>{{6, 4}});
	ret = check_property(list, 16, std::array<int, 2>{{6, 5}});
	ret = check_property(list, 18, std::array<int, 2>{{6, 5}});

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
				ret = 1;
			}
			continue;
		}
		else
		if (cmp == nullptr) {
			std::cerr << "error with " << i << std::endl;
			ret = 1;
			continue;
		}
		if (*ref != *cmp) {
			std::cerr << "error with " << i << std::endl;
			ret = 1;
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

	std::random_device rd;
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
		rd);

	lip.random_sets_with_properties(
		[](size_t const i)
		{
			return (rand()+i)%7;
		},
		6,
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
		rd);

	{
		// Test case for #3
		leeloo::list_intervals_with_properties<leeloo::list_intervals<interval>, property> lip;
		uint32_t const numbers[] = {0, 1412, 1419, 1861, 1868, 2358674, 2358837, 2360061, 2360265, 8257588, 16096644, 435833147, 446740354, 449990278, 539403484, 539403577, 540104153, 545351719, 548552882, 550653334, 550653338, 550662210, 550662620, 618113769, 618142111, 618276797, 687530529, 687533135, 687551925, 687552592, 955929212, 963130751, 1077367511, 1085914550, 1121688238, 1209241598, 1212694703, 1212694734, 1227123592, 1227125428, 1227151519, 1227189871, 1227215800, 1231355138, 1250295815, 1250295864, 1250295924, 1250296326, 1250298427, 1263125562, 1273503136, 1273504707, 1273504743, 1273505543, 1273506943, 1274722059, 1280643528, 1300727518, 1300727523, 1303811381, 1304058230, 1342710652, 1343216536, 1343217617, 1343231651, 1343242572, 1343243336, 1355605195, 1355621562, 1383029242, 1390616600, 1404885616, 1410784867, 1410785429, 1410785762, 1410786357, 1411663565, 1450157237, 1450200241, 1455615221, 1474908064, 1488122820, 1514897707, 1517836541, 1523420280, 1738834462, 2113276540, 2336414046, 2336437592, 2377122864, 2414978288, 2830636597, 2868866419, 2868900419, 2868900694, 2868900699, 2868900704, 2868907574, 2868913716, 2868919329, 2870331916, 2870354231, 2903929179, 2903966863, 2903993871, 2904019376, 2904019424, 2904019429, 2905982548, 2917477904, 2917481239, 2917481351, 3013270586, 3019420343, 3019420385, 3020404652, 3020564689, 3080472460, 3155746215, 3164262793, 3164262904, 3171221775, 3475425062, 3475763939, 3477015253, 3477015292, 3477722711, 3504740434, 3504751360, 3505512575, 3505512590, 3549552434, 3557644796, 3561360309};
		for (uint32_t n: numbers) {
			lip.add(n, n+1);
			lip.add_property(interval(n, n+1), {n});
		}
		lip.aggregate();
		lip.aggregate_properties_no_rem(
				[](property& org, property const& o)
				{
					for (int i: o) {
						org.push_back(i);
					}
				});
	}

	return ret;
}
