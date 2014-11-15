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

// Unique random number generator
// Inspired by http://preshing.com/20121224/how-to-generate-a-sequence-of-unique-random-integers

#ifndef LEELOO_UNI_H
#define LEELOO_UNI_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>
#include <atomic>

#ifndef _MSC_VER
#include <x86intrin.h>
#endif

#include <leeloo/atomic_helpers.h>
#include <leeloo/intrinsics.h>
#include <leeloo/prime_helpers.h>
#include <leeloo/integer_cast.h>
#include <leeloo/integer_traits.h>
#include <leeloo/uprng_base.h>

#ifdef LEELOO_BOOST_SERIALIZE
#include <boost/serialization/nvp.hpp>
#endif

namespace leeloo {

namespace __impl {

template <class integer_type>
struct seed_type_uni
{
	integer_type _off;
	integer_type _pos;
	integer_type _max;
	uint32_t _seed_perm;

	template <class Engine>
	static seed_type_uni random(integer_type const max, Engine& eng)
	{
		seed_type_uni ret;
		auto rand = leeloo::random_engine(eng);

		ret._off = rand.template uniform<integer_type>(0, max-1);
		ret._pos = rand.template uniform<integer_type>(0, max-1);
		ret._seed_perm = rand.template uniform<uint32_t>();
		ret._max = max;
		return ret;
	}

	integer_type const& off() const { return _off; }
	integer_type const& pos() const { return _pos; }
	integer_type const& max() const { return _max; }
	uint32_t seed_perm() const { return _seed_perm; }

#ifdef LEELOO_BOOST_SERIALIZE
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & boost::serialization::make_nvp("off", _off);
		ar & boost::serialization::make_nvp("pos", _pos);
		ar & boost::serialization::make_nvp("seed_perm", _seed_perm);
		ar & boost::serialization::make_nvp("max", _max);
	}
#endif
};

}

template <class Integer, bool atomic = false>
class uni: public uprng_base<uni<Integer, atomic>, Integer, __impl::seed_type_uni<Integer>>
{
	static_assert(std::is_signed<Integer>::value == false, "Integer must be an unsigned integer type.");
	//static_assert(sizeof(Integer) <= 4, "Integers wider than 32-bit integers aren't supported.");

public:
	typedef Integer integer_type;
	typedef __impl::seed_type_uni<integer_type> seed_type;
	typedef typename std::conditional<atomic, std::atomic<integer_type>, integer_type>::type pos_integer_type;
	typedef uprng_base<uni<Integer, atomic>, integer_type, seed_type> base_type;

public:
	using base_type::base_type;

	void init_base()
	{ 
		_rem_perm = nullptr;
	}

	~uni()
	{
		if (_rem_perm) {
			free(_rem_perm);
		}
	}

public:
	/*! Construct a Unique Random Integers (UNI) object.
	 *
	 * \param max defines the interval of the generated integers. max isn't included (between [0,max[).
	 */
	void init_seed(seed_type const& seed)
	{
		_intermediate_off = seed.off();
		_cur_pos = seed.pos();

		init_prime(seed.max());
		init_final_perm(seed);
	}

	inline integer_type max() const { return _max; }

public:
	integer_type operator()()
	{
		const integer_type pos = __impl::pos_increment(_cur_pos, _max);
		const integer_type res = residue((residue(pos) + _intermediate_off) % _max);
		return res;
	}

	inline integer_type get_step(integer_type const step) const
	{
		const integer_type real_step = (_cur_pos + step) % _max;
		return residue((residue(real_step) + _intermediate_off) % _max);
	}

private:
	integer_type residue(integer_type const v) const
	{
		typedef typename integer_above<integer_type>::type integer_above_type;
		assert(v < _max);
		const integer_type prime = _prime;
		if (v >= prime) {
			// Use the final permutation
			return _rem_perm[integer_cast<size_t>(v-prime)];
		}
		const integer_above_type vabove(v);
		const integer_type residue = integer_cast<integer_type>((vabove * vabove) % prime);
		return (v <= (prime / 2)) ? residue : prime - residue;
	}

private:
	void init_prime(integer_type const max)
	{
		_prime = find_previous_matching_prime(max);
		_max = max;
	}

	void init_final_perm(seed_type const& seed)
	{
		// Generate a random permutation for the final numbers
		if (_rem_perm) {
			free(_rem_perm);
		}
		const size_t rem = size_rem();
		posix_memalign((void**) &_rem_perm, 16, rem*sizeof(integer_type));
		assert(_rem_perm);
		for (size_t i = 0; i < rem; i++) {
			_rem_perm[i] = _prime + i;
		}

		std::mt19937 eng;
		eng.seed(seed.seed_perm());
		auto rand_eng = leeloo::random_engine(eng);
		std::random_shuffle(&_rem_perm[0], &_rem_perm[rem],
		                    [&rand_eng](size_t n) { return rand_eng.uniform<size_t>(0, n-1); });
	}
	
	inline size_t size_rem() const { return integer_cast<size_t>(_max-_prime); }

private:
	integer_type _prime;
	integer_type _max;
	integer_type _intermediate_off;
	integer_type* _rem_perm;
	pos_integer_type _cur_pos;
};

#ifdef __SSE4_2__
template <>
class uni<__m128i, false>
{
public:
	typedef __m128i integer_type;

public:
	uni():
		_rem_perm(nullptr)
	{
	}

	template <class Engine>
	uni(uint32_t const max, Engine const& rand_eng)
	{
		init(max, rand_eng);
	}

	~uni()
	{
		if (_rem_perm) {
			free(_rem_perm);
		}
	}

public:
	/*! Construct a Unique Random Integers (UNI) object.
	 *
	 * \param max defines the interval of the generated integers. max isn't included (between [0,max[).
	 */
	template <class Engine>
	void init(uint32_t const max, Engine const& rand_eng)
	{
		const uint32_t intermediate_off = rand_eng(0, max-1);
		const uint32_t cur_pos = rand_eng(0, max-1);

		_intermediate_off = _mm_set1_epi32(intermediate_off);
		_cur_pos = _mm_set_epi32((cur_pos+3)%max, (cur_pos+2)%max, (cur_pos+1)%max, cur_pos);

		init_prime(max);
		init_final_perm(rand_eng);
	}


public:
	__m128i operator()()
	{
		__m128i cur_pos = _cur_pos;
		const __m128i res = residue(_mm_urem_epi32(_mm_add_epi32(residue(cur_pos), _intermediate_off), _max));

		cur_pos = _mm_add_epi32(cur_pos, _mm_set1_epi32(4));
		const __m128i cmp = _mm_cmpgt_epi32(cur_pos, _mm_sub_epi32(_max, _mm_set1_epi32(1)));
		cur_pos = reinterpret_cast<__m128i>(_mm_blendv_ps(reinterpret_cast<__m128>(cur_pos), reinterpret_cast<__m128>(_mm_sub_epi32(cur_pos, _max)), reinterpret_cast<__m128>(cmp)));

		_cur_pos = cur_pos;

		return res;
	}

	__m128i get_step(uint32_t const step) const
	{
		__m128i cur_pos = _cur_pos;
		cur_pos = _mm_add_epi32(cur_pos, _mm_set1_epi32(step*4));
		const __m128i cmp = _mm_cmpgt_epi32(cur_pos, _mm_sub_epi32(_max, _mm_set1_epi32(1)));
		cur_pos = reinterpret_cast<__m128i>(_mm_blendv_ps(reinterpret_cast<__m128>(cur_pos), reinterpret_cast<__m128>(_mm_sub_epi32(cur_pos, _max)), reinterpret_cast<__m128>(cmp)));

		const __m128i res = residue(_mm_urem_epi32(_mm_add_epi32(residue(cur_pos), _intermediate_off), _max));

		return res;
	}

private:
	integer_type residue(__m128i const v) const
	{
		const uint64_t prime = _prime;
		__m128i prime_sse = _mm_set1_epi32(prime);
		__m128i cmp = _mm_cmpgt_epi32(v, _mm_sub_epi32(prime_sse, _mm_set1_epi32(1)));
		if (!_mm_test_all_zeros(cmp, _mm_set1_epi32(0xFFFFFFFF))) {
			const uint32_t prime_d2 = prime>>1;
			uint32_t org_v;
#define ITER(vi, i)\
			org_v = _mm_extract_epi32(v, i);\
			uint32_t vi;\
			if (_mm_extract_epi32(cmp, i)) {\
				vi = _rem_perm[org_v-prime];\
			}\
			else {\
				vi = ((uint64_t) org_v * (uint64_t) org_v) % prime;\
				if (org_v > prime_d2) {\
					vi = prime - vi;\
				}\
			}\

			ITER(v0, 0);
			ITER(v1, 1);
			ITER(v2, 2);
			ITER(v3, 3);

			return _mm_set_epi32(v3, v2, v1, v0);
		}

		// Let v = [v0, v1, v2, v3].
		// Two passes here. The first mul will multiply v0 and v2.
		// Then we swap 'v' and do it again.
		__m128i res_comp;
		cmp = _mm_cmpgt_epi32(v, _mm_srli_epi32(prime_sse, 1));
		prime_sse = _mm_unpacklo_epi32(prime_sse, _mm_setzero_si128());

		uint64_t tmp0, tmp1;

#define RESIDUE(resi, v_, cmp_)\
		__m128i resi;\
		resi = _mm_mul_epu32(v_, v_);\
		tmp0 = _mm_extract_epi64(resi, 0) % prime;\
		tmp1 = _mm_extract_epi64(resi, 1) % prime;\
		resi = _mm_insert_epi64(_mm_insert_epi64(_mm_setzero_si128(), tmp0, 0), tmp1, 1);\
		res_comp = _mm_sub_epi64(prime_sse, resi);\
		resi = reinterpret_cast<__m128i>(_mm_blendv_pd(reinterpret_cast<__m128d>(resi), reinterpret_cast<__m128d>(res_comp), reinterpret_cast<__m128d>(cmp_)));

		const __m128i cmpswap = _mm_shuffle_epi32(cmp, 2 << 6);
		RESIDUE(res0, v, cmpswap);

		const __m128i vswap = _mm_shuffle_epi32(v, 1 | (3 << 4));
		RESIDUE(res1, vswap, cmp);

		return _mm_unpacklo_epi32(_mm_shuffle_epi32(res0, 2 << 2),
		                          _mm_shuffle_epi32(res1, 2 << 2));
	}

private:
	void init_prime(uint32_t const max)
	{
		_prime = find_previous_matching_prime(max);
		_max = _mm_set1_epi32(max);
	}

	template <class Engine>
	void init_final_perm(Engine const& rand_eng)
	{
		// Generate a random permutation for the final numbers
		const uint32_t rem = size_rem();
		posix_memalign((void**) &_rem_perm, 16, rem*sizeof(integer_type));
		assert(_rem_perm);
		for (uint32_t i = 0; i < rem; i++) {
			_rem_perm[i] = _prime + i;
		}

		std::random_shuffle(&_rem_perm[0], &_rem_perm[rem],
		                    [&rand_eng](uint32_t n) { return rand_eng(0, n-1); });
	}
	
	inline uint32_t size_rem() const { return _mm_extract_epi32(_max, 0) - _prime; }

private:
	uint32_t _prime;
	__m128i _max;
	__m128i _intermediate_off;
	uint32_t* _rem_perm;
	__m128i _cur_pos;
};
#endif

}

#endif
