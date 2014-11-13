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
#include <ctime>

#include <leeloo/helpers.h>
#include <leeloo/bench.h>
#include <leeloo/uni.h>
#include <leeloo/random.h>

bool check(std::vector<uint32_t>& res, const size_t n)
{
	std::sort(res.begin(), res.end());
	auto it_end = std::unique(res.begin(), res.end());
	uint32_t v = 0;

	bool ret = true;

	if (std::distance(res.begin(), it_end) != (ssize_t) n) {
		std::cerr << "Error: number of elements invalid!" << std::endl;
		ret = false;
	}

	for (auto it = res.begin(); it != it_end; it++) {
		if (*it != v) {
			std::cerr << "Error: " << *it << " != " << v << std::endl;
			ret = false;
		}
		v++;
	}

	return ret;
}

int main(int argc, char** argv)
{
	size_t n = (argc > 1) ? atoll(argv[1]) : 20;
	
	std::random_device gen;
	leeloo::uni<uint32_t> uni;
	uni.init(n, gen);

	std::vector<uint32_t> res;
	res.resize(n);
	BENCH_START(rand);
	for (size_t i = 0; i < n; i++) {
		res[i] = uni();
	}
	BENCH_END(rand, "rand-serial", 1, 1, sizeof(uint32_t), n);

	if (!check(res, n)) {
		return 1;
	}
	
	memset(&res[0], 0, sizeof(uint32_t)*n);

	BENCH_START(parallel2);
#pragma omp parallel for
	for (size_t i = 0; i < n; i++) {
		res[i] = uni.get_step(i);
	}
	BENCH_END(parallel2, "rand-parallel2", 1, 1, sizeof(uint32_t), n);
	
	if (!check(res, n)) {
		return 1;
	}

#ifdef __SSE4_2__
	leeloo::uni<__m128i> uni_sse;
	uni_sse.init(n, leeloo::random_engine<uint32_t>(gen));

	memset(&res[0], 0, sizeof(uint32_t)*n);

	BENCH_START(sse);
	for (size_t i = 0; i < n/4; i++) {
		__m128i res_ = uni_sse();
		_mm_store_si128((__m128i*)&res[i*4], res_);
	}
	BENCH_END(sse, "rand-sse", 1, 1, sizeof(uint32_t), n);

	if (!check(res, n & ~3U)) {
		return 1;
	}

	memset(&res[0], 0, sizeof(uint32_t)*n);

	BENCH_START(parallel_sse);
#pragma omp parallel for
	for (size_t i = 0; i < n/4; i++) {
		const __m128i res_ = uni_sse.get_step(i);
		_mm_store_si128((__m128i*)&res[i*4], res_);
	}
	BENCH_END(parallel_sse, "rand-parallel-sse", 1, 1, sizeof(uint32_t), n);

	if (!check(res, n & ~3U)) {
		return 1;
	}
#endif

	return 0;
}
