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

#ifndef LEELOO_UPRNG_H
#define LEELOO_UPRNG_H

#include <type_traits>
#include <cmath>
#include <algorithm>
#include <limits>
#include <cassert>

#include <x86intrin.h>

#include <tbb/atomic.h>

#include <leeloo/intrinsics.h>
#include <leeloo/atomic_helpers.h>
#include <leeloo/prime_helpers.h>
#include <leeloo/random.h>
#include <leeloo/uprng_base.h>
#include <leeloo/integer_traits.h>
#include <leeloo/integer_cast.h>

#ifdef LEELOO_BOOST_SERIALIZE
#include <boost/serialization/nvp.hpp>
#endif

namespace leeloo {

namespace __impl {

template <class IntegerType>
struct seed_type_uprng
{
	typedef IntegerType integer_type;

	integer_type _a;
	integer_type _b;
	integer_type _c;
	integer_type _max;
	integer_type _prime;
	uint8_t _n;

	integer_type const& max() const { return _max; }

	template <class Engine>
	static seed_type_uprng random(integer_type const max, Engine& eng)
	{
		seed_type_uprng ret;
		auto rand_eng = leeloo::random_engine(eng);
		ret._a = rand_eng.template uniform<integer_type>(1, max-1);
		ret._b = rand_eng.template uniform<integer_type>(0, max-1);
		ret._prime = find_next_prime(max);
		assert(ret._prime != 0);

		ret._c = random_prime_with(ret._prime-1, rand_eng);
		ret._n = rand_eng.template uniform<uint8_t>(1, 4);
		ret._max = max;
		return ret;
	}

#ifdef LEELOO_BOOST_SERIALIZE
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & boost::serialization::make_nvp("a", _a);
		ar & boost::serialization::make_nvp("b", _b);
		ar & boost::serialization::make_nvp("c", _c);
		ar & boost::serialization::make_nvp("max", _max);
		ar & boost::serialization::make_nvp("prime", _prime);
		ar & boost::serialization::make_nvp("n", _n);
	}
#endif
};

}

template <class Integer, bool atomic = false>
class uprng: public uprng_base<uprng<Integer, atomic>, Integer, __impl::seed_type_uprng<Integer>>
{
	static_assert(std::is_signed<Integer>::value == false, "Integer must be an unsigned integer type.");

public:
	typedef Integer integer_type;
	typedef typename std::conditional<atomic, tbb::atomic<integer_type>, integer_type>::type pos_integer_type;
	typedef __impl::seed_type_uprng<integer_type> seed_type;

public:
	void init_base() { }

	/*! Construct a UPRNG object.
	 *
	 * \param max defines the interval of the generated integers. max isn't included (between [0,max[).
	 */
	void init_seed(seed_type const& seed)
	{
		_max = seed.max();

		_a = seed._a;
		_b = seed._b;

		_c = seed._c;
		_n = seed._n;

		_prime = seed._prime;

		_cur_step = 0;
	}


public:
	inline integer_type operator()()
	{
		integer_type res;
		do {
			const integer_type pos = __impl::pos_increment(_cur_step, _prime);
			res = get_step(pos);
		}
		while (res >= _max);

		return res;
	}

	inline integer_type get_step(integer_type const step) const
	{
		integer_type ret = step;
		for (uint8_t i = 0; i < _n; i++) {
			ret = g(l(ret, _a, _b, _prime), _c, _prime);
		}
		return ret;
	}

private:
	inline static integer_type l(integer_type const X, integer_type const a, integer_type const b, integer_type const p)
	{
		typedef typename integer_above<integer_type>::type integer_above_type;
		return integer_cast<integer_type>((integer_cast<integer_above_type>(a)*integer_cast<integer_above_type>(X)+integer_cast<integer_above_type>(b))%p);
	}

	inline static integer_type g(integer_type const X, integer_type const c, integer_type const p)
	{
		return exp_mod(X, c, p);
	}

private:
	integer_type _prime;
	integer_type _max;
	integer_type _a;
	integer_type _b;
	integer_type _c;
	uint8_t _n;
	pos_integer_type _cur_step;
};

}

#endif
