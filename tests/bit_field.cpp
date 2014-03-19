#include <leeloo/bit_field.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>

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

	return ret;
}
