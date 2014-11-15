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

#ifndef LEELOO_INTRINSICS_H
#define LEELOO_INTRINSICS_H

#ifdef __SSE4_2__
#include <x86intrin.h>

inline static __m128i _mm_urem_epi32(__m128i const a, __m128i const div)
{
#ifdef __AVX__
	const __m256d a_dble = _mm256_cvtepi32_pd(a);
	const __m256d div_dble = _mm256_cvtepi32_pd(div);
	const __m256d integer_part = _mm256_floor_pd(_mm256_div_pd(a_dble, div_dble));
	const __m256d res_dble = _mm256_sub_pd(a_dble, _mm256_mul_pd(integer_part, div_dble));

	return _mm256_cvttpd_epi32(res_dble);

#else
	return _mm_set_epi32(_mm_extract_epi32(a, 3) % _mm_extract_epi32(div, 3),
	                     _mm_extract_epi32(a, 2) % _mm_extract_epi32(div, 2),
	                     _mm_extract_epi32(a, 1) % _mm_extract_epi32(div, 1),
	                     _mm_extract_epi32(a, 0) % _mm_extract_epi32(div, 0));
#endif
}

inline static __m128i _mm_mulmod_epu32(__m128i const a, __m128i const b, uint64_t const m)
{
	uint64_t tmp0, tmp1;
#define LEELOO__MUL__(resi, a_, b_)\
	__m128i resi;\
	resi = _mm_mul_epu32(a_, b_);\
	tmp0 = _mm_extract_epi64(resi, 0) % m;\
	tmp1 = _mm_extract_epi64(resi, 1) % m;\
	resi = _mm_insert_epi64(_mm_insert_epi64(_mm_setzero_si128(), tmp0, 0), tmp1, 1);\

	LEELOO__MUL__(res0, a, b);

	const __m128i aswap = _mm_shuffle_epi32(a, 1 | (3 << 4));
	const __m128i bswap = _mm_shuffle_epi32(b, 1 | (3 << 4));
	LEELOO__MUL__(res1, aswap, bswap);

	return _mm_unpacklo_epi32(_mm_shuffle_epi32(res0, 2 << 2),
			_mm_shuffle_epi32(res1, 2 << 2));
}

inline static __m128i _mm_mulmodadd_epu32(__m128i const a, __m128i const b, __m128i const c, uint64_t const m)
{
	uint64_t tmp0, tmp1;
#define LEELOO__MULADD__(resi, a_, b_)\
	__m128i resi;\
	resi = _mm_add_epi32(_mm_mul_epu32(a_, b_), c);\
	tmp0 = _mm_extract_epi64(resi, 0) % m;\
	tmp1 = _mm_extract_epi64(resi, 1) % m;\
	resi = _mm_insert_epi64(_mm_insert_epi64(_mm_setzero_si128(), tmp0, 0), tmp1, 1);\

	LEELOO__MULADD__(res0, a, b);

	const __m128i aswap = _mm_shuffle_epi32(a, 1 | (3 << 4));
	const __m128i bswap = _mm_shuffle_epi32(b, 1 | (3 << 4));
	LEELOO__MULADD__(res1, aswap, bswap);

	return _mm_unpacklo_epi32(_mm_shuffle_epi32(res0, 2 << 2),
			_mm_shuffle_epi32(res1, 2 << 2));
}

#endif

#endif
