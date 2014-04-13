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
#include <math.h>

#include <boost/random.hpp>

#include <leeloo/interval.h>
#include <leeloo/list_intervals.h>
#include <leeloo/random.h>
#include <leeloo/uprng.h>

template <class Interval>
void print_intervals(Interval const& l)
{
	typedef typename Interval::interval_type interval_type;
	for (interval_type const& i: l.intervals()) {
		std::cout << i.lower() << " " << i.upper() << std::endl;
	}
}

typedef leeloo::list_intervals<leeloo::interval<uint32_t>> list_intervals;

int main(int argc, char** argv)
{
	const size_t n = argc >= 2 ? atoll(argv[1]) : 1031;

	list_intervals list;
	unsigned int seed = time(NULL);
	srand(seed);

	std::cout << "Generate random intervals with seed " << seed << "..." << std::endl;
	// Generate random intervals
	for (size_t i = 0; i < n; i++) {
		uint32_t a = rand();
		if ((a + 60) >= a) {
			a -= 60;
		}
		const uint32_t b = a + (rand()%((rand()%60) + 1));
		list.add(a, b);
	}
	for (size_t i = 0; i < sqrt(n); i++) {
		uint32_t a = rand();
		if ((a + 60) >= a) {
			a -= 60;
		}
		const uint32_t b = a + (rand()%((rand()%20) + 1));
		list.remove(a, b);
	}
	std::cout << "Done." << std::endl;

	// Aggregate them
	BENCH_START(agg);
	list.aggregate();
	BENCH_END_NODISP(agg);
	const double time_agg = BENCH_END_TIME(agg);
	fprintf(stderr, "Aggregate in %0.4fms: %0.4f intervals/s\n", time_agg*1000.0, (double)n/(time_agg));

	std::cout << "Size all: " << list.size() << std::endl;

	boost::random::mt19937 mt_rand(time(NULL));
	size_t size_sets = 0;
	list.create_index_cache(2048);
	BENCH_START(random_sets);
	list.random_sets<leeloo::uprng>(16,
		[&size_sets](uint32_t const* /*ints*/, const ssize_t size)
		{
			size_sets += size;
		},
		leeloo::random_engine<uint32_t>(mt_rand));
	BENCH_END(random_sets, "random_sets", 1, 1, sizeof(uint32_t), list.size());

	if (size_sets != list.size()) {
		std::cerr << "Random sets returns " << size_sets << " results, expected " << list.size() << std::endl;
		return 1;
	}
	else {
		std::cout << "Size all sets: " << size_sets << std::endl;
	}
	               
	return 0;
}
