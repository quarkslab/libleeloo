#include <leeloo/sort_permute.h>
#include <leeloo/bit_field.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <algorithm>

#define N 1007

int main()
{
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

	srand(time(NULL));
	bf.clear_all();
	for (size_t i = 0; i < N; i++) {
		if (rand() & 1) {
			bf.set_bit(i);
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

	int ar1[] = {6, 5, 4, 3, 2};
	const size_t n = sizeof(ar1)/sizeof(int);
	leeloo::bit_field bf2(n);
	bf2.clear_all();
	bf2.set_bit(0);
	for (size_t i = 0; i < n; i++) {
		std::cout << bf2.get_bit_fast(i) << std::endl;
	}

	std::cout << "sort" << std::endl;
	std::sort(leeloo::make_sort_permute_iter(ar1, bf2.begin()), leeloo::make_sort_permute_iter(ar1+n, bf2.begin()+n), leeloo::sort_permute_iter_compare<int*, int*>());
	for (int i: ar1) {
		std::cout << i << std::endl;
	}
	for (size_t i = 0; i < n; i++) {
		std::cout << bf2.get_bit_fast(i) << std::endl;
	}

	return ret;
}
