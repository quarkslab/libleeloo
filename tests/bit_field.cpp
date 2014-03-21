#include <leeloo/sort_permute.h>
#include <leeloo/bit_field.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <algorithm>

#define N 100007

int main(int argc, char** argv)
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

	bf.set_random();
	ssize_t idxes[] = {100, 67, -49, 62, 1000, -1007, 5000, -654, -50, -51, 90};
	it = bf.begin();
	ssize_t idx = 0;
	size_t i = 0;
	it += 167;
	it -= 49;
	it += 62;
	leeloo::bit_field::iterator it_cmp = bf.begin()+(167-49+62);
	if (it != it_cmp) {
		std::cout << "LOL" << std::endl;
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

	int n = atoi(argv[1]);
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
		std::cout << "error copy" << std::endl;
	}

	bf2.set_random();
	bf2_ref = bf2;
	std::cout << "sort " << ar1 << std::endl;
	std::sort(leeloo::make_sort_permute_iter(ar1, bf2.begin()), leeloo::make_sort_permute_iter(ar1+n, bf2.begin()+n), leeloo::sort_permute_iter_compare<int*, leeloo::bit_field::iterator>());
	for (int i = 0; i < n; i++) {
		if (ar1[i] != i) {
			std::cout << "error array" << std::endl;
		}
	}
	if (bf2 != bf2_ref) {
		std::cout << "error" << std::endl;
	}

	return ret;
}
