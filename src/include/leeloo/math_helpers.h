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

#ifndef LEELOO_MATH_HELPERS_H
#define LEELOO_MATH_HELPERS_H

namespace leeloo {

template <class Integer>
inline Integer log2i(Integer const v)
{
	static_assert(sizeof(Integer) <= 4, "Integers wider than 32-bit integers aren't supported.");

	if (v == 0) {
		return 0;
	}

	return _bit_scan_reverse(v);
}

template <class Integer>
inline Integer is_pow2(Integer const v)
{
	return (((Integer)1)<<log2i(v)) == v;
}

template <class Integer>
Integer exp_mod(Integer v, Integer e, Integer const m)
{
	Integer r = 1;
	while (e > 0) {
		if ((e & 1) == 1) {
			r = ((uint64_t)r*(uint64_t)v) % m;
		}
		e >>= 1;
		v = ((uint64_t)v*(uint64_t)v) % m;
	}
	return r;
}

#if 0
Integer mm_exp_mod_epi32(__m128i v, uint32_t e, __m128i const m)
{
	__m128i r = _mm_set1_epi32(1);
	while (e > 0) {
		if ((e & 1) == 1) {
			r = ((uint64_t)r*(uint64_t)v) % m;
		}
		e >>= 1;
		v = ((uint64_t)v*(uint64_t)v) % m;
	}
	return r;
}
#endif

template <class Integer>
Integer gcd(Integer a, Integer b)
{
	Integer c;
	while (a != 0) {
		c = a;
		a = b%a;
		b = c;
	}
	return b;
}

}

#endif
