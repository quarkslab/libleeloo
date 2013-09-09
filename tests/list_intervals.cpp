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
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <set>

#include <boost/random.hpp>

#include <leeloo/interval.h>
#include <leeloo/list_intervals.h>

template <class Interval>
void print_intervals(Interval const& l)
{
	typedef typename Interval::interval_type interval_type;
	for (interval_type const& i: l.intervals()) {
		std::cout << i.lower() << " " << i.upper() << std::endl;
	}
}

// Interval of type [a,b[
typedef leeloo::list_intervals<leeloo::interval<uint32_t>, uint32_t> list_intervals;

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " n" << std::endl;
		return 1;
	}

	list_intervals list;
	list.add(0, 2);
	list.add(5, 9);
	list.add(1, 5);
	list.add(8, 9);
	list.add(9, 15);
	list.add(19, 21);

	std::cout << list.at(0) << " == 0" << std::endl;
	std::cout << list.at(1) << " == 1" << std::endl;
	std::cout << list.at(2) << " == 5" << std::endl;
	std::cout << list.at(3) << " == 6" << std::endl;
	std::cout << "contains 1 (true) == " << list.contains(1) << std::endl;
	std::cout << "contains 2 (true) == " << list.contains(2) << std::endl;
	std::cout << "contains 6 (true) == " << list.contains(6) << std::endl;
	std::cout << "contains 19 (true) == " << list.contains(19) << std::endl;
	std::cout << "contains 40 (false) == " << list.contains(40) << std::endl;

	print_intervals(list);

	list.aggregate();

	std::cout << "After aggregate:" << std::endl;
	
	print_intervals(list);
	std::cout << "size: " << list.size() << std::endl;

	const size_t n = atoll(argv[1]);

	list.clear();
	list.reserve(n);

	srand(0);
	//srand(time(NULL));

	std::cout << "Generate random intervals..." << std::endl;
	// Generate random intervals
	for (size_t i = 0; i < n; i++) {
		const uint32_t a = rand();
		const uint32_t b = a + (rand()%((rand()%600) + 1) + 1);
		list.add(a, b);
	}
	std::cout << "Done." << std::endl;

	list.aggregate();

	list_intervals::container_type const& intervals = const_cast<list_intervals const&>(list).intervals();

	for (size_t i = 0; i < intervals.size(); i++) {
		const uint32_t vcontain0 = intervals[i].lower();
		const uint32_t vcontain1 = intervals[i].middle();
		const uint32_t vcontain2 = intervals[i].upper();
		if (!list.contains(vcontain0) || !list.contains(vcontain1) || list.contains(vcontain2)) {
			std::cerr << "Error: contains returns invalid results!" << std::endl; 
			return 1;
		}
	}

	// Checked cached item access
	list.create_index_cache(16);

	const uint32_t size_all = list.size();
	std::cout << "Size all: " << size_all << std::endl;
	for (uint32_t i = 0; i < size_all; i++) {
		if (i == 34327) {
			std::cerr << "bug here" << std::endl;
		}
		const uint32_t v0 = list.at(i);
		const uint32_t v1 = list.at_cached(i);
		if (v0 != v1) {
			std::cerr << "Error in at_cached: i=" << i << ", got " << v1 << ", should be " << v0 << std::endl;
		}
	}

	std::cout << "[0,10[ - [20,21[" << std::endl;
	list.clear();
	list.add(0, 10);
	list.remove(20, 21);
	list.aggregate();
	print_intervals(list);

	std::cout << "[0,10[ - [5,6[" << std::endl;
	list.clear();
	list.add(0, 10);
	list.remove(5, 6);
	list.aggregate();
	print_intervals(list);

	std::cout << "[0,10[ - [0,10[" << std::endl;
	list.clear();
	list.add(0, 10);
	list.remove(0, 10);
	list.aggregate();
	print_intervals(list);

	std::cout << "[10,50[ - [8,11[" << std::endl;
	list.clear();
	list.add(10, 50);
	list.remove(8, 11);
	list.aggregate();
	print_intervals(list);

	std::cout << "[10,50[ - [40,54[" << std::endl;
	list.clear();
	list.add(10, 50);
	list.remove(40, 54);
	list.aggregate();
	print_intervals(list);

	std::cout << "[10,50[ - [50,54[" << std::endl;
	list.clear();
	list.add(10, 50);
	list.remove(50, 54);
	list.aggregate();
	print_intervals(list);

	std::cout << "[0,10[ - ([5,6[,[7,8[,[3,4[" << std::endl;
	list.clear();
	list.add(0, 10);
	list.remove(5, 6);
	list.remove(7, 8);
	list.remove(3, 4);
	list.aggregate();
	print_intervals(list);

	std::cout << "([0,10[,[20,40[) - [5,29[" << std::endl;
	list.clear();
	list.add(0, 10);
	list.add(20, 40);
	list.remove(5,29);
	list.aggregate();
	print_intervals(list);

	std::cout << "([0,10[,[20,40[,[50,90[,[100,200[) - ([0,4[, [5,21[, [25,29[, [30,35[,[36,105[" << std::endl;
	list.clear();
	list.add(0, 10);
	list.add(20, 40);
	list.add(50, 90);
	list.add(100, 200);
	list.remove(0, 4);
	list.remove(5, 21);
	list.remove(25, 29);
	list.remove(30, 35);
	list.remove(36, 105);
	list.aggregate();
	print_intervals(list);

	std::cout << "---------" << std::endl;

	list.clear();
	for (size_t i = 0; i < 20; i++) {
		list.add(i*10, ((i+1)*10) - 1);
	}
	list.remove(51,52);
	list.aggregate();
	print_intervals(list);

	return 0;
}
