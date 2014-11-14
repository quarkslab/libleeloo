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
	if (argc <= 2) {
		std::cerr << "Usage: " << argv[0] << " n mean_size" << std::endl;
		return 1;
	}

	list_intervals list;

	const size_t n = atoll(argv[1]);
	const size_t mean_size = atoll(argv[2]);

	list.clear();
	list.reserve(n);

	srand(time(NULL));

	std::cout << "Generate random intervals..." << std::endl;
	// Generate random intervals
	for (size_t i = 0; i < n; i++) {
		const uint32_t a = rand();
		const uint32_t b = a + (rand()%((rand()%(2*mean_size)) + 1));
		list.add(a, b);
	}
	std::cout << "Done." << std::endl;

	list.aggregate();

	// Checked cached item access
	BENCH_START(cache);
	list.create_index_cache(16);
	BENCH_END(cache, "create_index_cache", 1, 1, 1, 1);

	const uint32_t size_all = list.size();
	std::cout << "Performances of getting the last element:" << std::endl;

	list.at(size_all-1);
	BENCH_START(at);
	list.at(size_all-1);
	BENCH_END(at, "at", sizeof(list_intervals::interval_type), n, 1, sizeof(uint32_t));

	list.at_cached(size_all-1);
	BENCH_START(at_cached);
	list.at_cached(size_all-1);
	BENCH_END(at_cached, "at-cached", sizeof(list_intervals::interval_type), n, 1, sizeof(uint32_t));

	return 0;
}
