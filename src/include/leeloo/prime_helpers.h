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

#ifndef LEELOO_PRIME_HELPERS_H
#define LEELOO_PRIME_HELPERS_H

#include <x86intrin.h>
#include <leeloo/math_helpers.h>
#include <leeloo/random.h>

namespace leeloo {

template <class Integer>
bool is_prime(Integer const v)
{
	// Don't check with two because we only check odd numbers
	Integer const v_sqrt = sqrt(v);
	for (Integer d = 3; d <= v_sqrt; d++) {
		// TODO: does gcc vectorize this ?
		if ((v % d) == 0) {
			return false;
		}
	}
	return true;
}

template <class Integer>
Integer find_previous_matching_prime(Integer const v)
{
	Integer p;
	if ((v & 1) == 0) {
		p = v - 1;
	}
	else {
		p = v;
	}

	for (; p >= 3; p -= 2) {
		if (is_prime(p) && ((p&3) == 3)) {
			return p;
		}
	}

	return 0;
}

template <class Integer>
Integer find_next_prime(Integer const v)
{
	Integer p;
	if ((v & 1) == 0) {
		p = v + 1;
	}
	else {
		p = v;
	}

	while (true) {
		if (is_prime(p)) {
			return p;
		}
		p += 2;
	}

	return 0;
}

template <class Integer, class Engine>
Integer random_prime_with(Integer const n, Engine& rand_eng)
{
	Integer ret, g;
	do {
		ret = rand_eng.template uniform<Integer>(0, n-2);
		g = gcd(ret, n);
	}
	while (g != 1);

	return ret;
}

}

#endif
