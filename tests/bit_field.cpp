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

#include <leeloo/sort_permute.h>
#include <leeloo/bit_field.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <algorithm>

#define N 100007

int test_random_access()
{
	int ret = 0;
	for (size_t nbits = 1100; nbits <= 1111; nbits++) {
		leeloo::bit_field bf(nbits);
		bf.clear_all();
		for (size_t j = 0; j < nbits; j++) {
			for (size_t i = 0; i < nbits-j; i++) {
				bf.set_bit(i+j);
				leeloo::bit_field::iterator it = bf.begin();
				it += j;
				it += i;
				leeloo::bit_field::iterator it2 = bf.begin();
				it2 += i;
				it2 += j;
				if (*it != true || *it2 != true) {
					std::cerr << "error forward random access with j=" << j << ", i=" << i << std::endl;
					ret = 1;
				}
				bf.clear_bit(i+j);
			}
		}

		leeloo::bit_field::iterator it_last = bf.begin()+(nbits-1);
		for (size_t j = 0; j < nbits; j++) {
			for (size_t i = 0; i < nbits-j; i++) {
				const size_t bit = nbits-(i+j)-1;
				bf.set_bit(bit);
				leeloo::bit_field::iterator it = it_last;
				it -= j;
				it -= i;
				leeloo::bit_field::iterator it2 = it_last;
				it2 -= j;
				it2 -= i;
				if (*it != true || *it2 != true) {
					std::cerr << "error reverse random access with j=" << j << ", i=" << i << std::endl;
					ret = 1;
				}
				bf.clear_bit(bit);
			}
		}
	}
	return ret;
}

int test_reserve()
{
	// This was the root cause of https://github.com/quarkslab/libleeloo/issues/3
	// Ridiculous bug, but still, the reserve function wasn't tested...
	//
	
	leeloo::bit_field bf;
	for (int i = 0; i < 100; i++) {
		bf.set_bit(i);
	}
	for (int i = 0; i < 100; i++) {
		if (!bf.get_bit(i)) {
			std::cout << "potential error with the reserve function!" << std::endl;
			return 1;
		}
	}
	return 0;
}

int main()
{
	srand(time(NULL));

	int ret = 0;
	leeloo::bit_field bf(N);
	bf.set_all_even();
	leeloo::bit_field::iterator it = bf.begin();
	for (size_t i = 0; i < N; i++) {
		const bool v = *it;
		const bool ref = !(i&1);
		if (v != ref) {
			std::cerr << "error with it " << i << std::endl;
			ret = 1;
		}
		if (bf.get_bit_fast(i) != ref) {
			std::cerr << "error with get_bit_fast " << i << std::endl;
			ret = 1;
		}
		it++;
	}

	ret = test_reserve();
	ret = test_random_access();

	bf.set_random();

	{
		it = bf.begin();
		it += 167;
		it -= 49;
		it += 62;
		leeloo::bit_field::iterator it_cmp = bf.begin()+(167-49+62);
		if (it != it_cmp) {
			std::cerr << "error random access with iterators" << std::endl;
		}
	}

	{
		it = bf.begin();
		it += 1245;
		it -= 456;
		it += 777;
		it -= 17;
		it += 2179;
		leeloo::bit_field::iterator it_cmp = bf.begin()+(1245-456+777-17+2179);
		if (it != it_cmp) {
			std::cerr << "error random access with iterators" << std::endl;
		}
	}

	it = bf.begin();
	for (size_t i = 0; i < N; i++) {
		leeloo::bit_field::iterator it_adv = it;
		it_adv += i;
		if (*it_adv != bf.get_bit_fast(i)) {
			std::cerr << "error with adv " << i << std::endl;
			ret = 1;
		}
		bool ref = *it_adv;
		*it_adv = !(*it_adv);
		if (*it_adv == ref) {
			std::cerr << "error with adv " << i << std::endl;
			ret = 1;
		}
	}

	// Reverse
	it = bf.begin()+N;
	for (size_t i = N; i > 0; i--) {
		leeloo::bit_field::iterator it_adv = it;
		it_adv -= i;
		if (*it_adv != bf.get_bit_fast(N-i)) {
			std::cerr << "error with negative adv " << i << std::endl;
			ret = 1;
		}
	}

	// Iterator assignation
	bf.clear_all();
	it = bf.begin();
	*it = true;
	if (bf.get_bit_fast(0) != true) {
		std::cerr << "error iterator assignation immediate" << std::endl;
	}
	std::cout << (*(bf.begin()+1)) << std::endl;
	*it = *(bf.begin()+1);
	if (bf.get_bit_fast(0) != false) {
		std::cerr << "error iterator assignation iterator" << std::endl;
	}

	int n = 127;
	int* ar1 = new int[n];
	for (int i = 0; i < n; i++) {
		ar1[i] = i;
	}
	leeloo::bit_field bf2(n);
	bf2.set_all();
	leeloo::bit_field bf2_ref = bf2;

	leeloo::bit_field bf2_test(n);
	bf2_test.clear_all();
	leeloo::bit_field::iterator it_res = bf2_test.begin();
	for (leeloo::bit_field::iterator it = bf2.begin(); it != bf2.end(); it++) {
		*it_res = *it;
		it_res++;
	}

	if (bf2_test != bf2_ref) {
		std::cerr << "error with copy" << std::endl;
	}

	bf2.set_random();
	bf2_ref = bf2;
	std::sort(leeloo::make_sort_permute_iter(ar1, bf2.begin()), leeloo::make_sort_permute_iter(ar1+n, bf2.begin()+n), leeloo::sort_permute_iter_compare<int*, leeloo::bit_field::iterator>());
	for (int i = 0; i < n; i++) {
		if (ar1[i] != i) {
			std::cerr << "error array at index " << i << std::endl;
		}
	}
	if (bf2 != bf2_ref) {
		std::cerr << "error sort" << std::endl;
	}

	return ret;
}
