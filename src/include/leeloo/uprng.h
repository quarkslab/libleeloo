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

#include <x86intrin.h>

#include <boost/random.hpp>

#include <tbb/atomic.h>

#include <leeloo/intrinsics.h>
#include <leeloo/atomic_helpers.h>
#include <leeloo/prime_helpers.h>

namespace leeloo {

template <class Integer, bool atomic = false>
class uprng
{
	static_assert(std::is_signed<Integer>::value == false, "Integer must be an unsigned integer type.");
	static_assert(sizeof(Integer) <= 4, "Integers wider than 32-bit integers aren't supported.");

public:
	typedef Integer integer_type;
	typedef typename std::conditional<atomic, tbb::atomic<integer_type>, integer_type>::type pos_integer_type;

public:
	uprng()
	{
	}

	template <class Engine>
	uprng(integer_type const max, Engine& rand_eng)
	{
		init(max, rand_eng);
	}

	~uprng()
	{
	}

public:
	/*! Construct a UPRNG object.
	 *
	 * \param max defines the interval of the generated integers. max isn't included (between [0,max[).
	 */
	template <class Engine>
	void init(integer_type const max, Engine& rand_eng)
	{
		_max = max;

		_a = rand_eng(1, max-1);
		_b = rand_eng(0, max-1);

		init_prime(max);
		_c = random_prime_with(_prime-1, rand_eng);
		_n = rand_eng(1, 4);

		std::cout << max << " " << _prime << " " << _a << " " << _b << " " << _c << " " << _n << std::endl;

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
		for (integer_type i = 0; i < _n; i++) {
			ret = g(l(ret, _a, _b, _prime), _c, _prime);
		}
		return ret;
	}

private:
	void init_prime(integer_type const max)
	{
		_prime = find_next_prime(max);
		assert(_prime != 0);
	}

	inline static integer_type l(integer_type const X, integer_type const a, integer_type const b, integer_type const p)
	{
		return ((uint64_t)a*(uint64_t)X+(uint64_t)b)%p;
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
	integer_type _n;
	pos_integer_type _cur_step;
};

}

#endif
