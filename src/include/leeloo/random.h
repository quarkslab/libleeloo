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

#ifndef LEELOO_RANDOM_H
#define LEELOO_RANDOM_H

#include <leeloo/config.h>

#ifdef LEELOO_MP_SUPPORT
#include <boost/multiprecision/random.hpp>
#elif defined HAS_BOOST_RANDOM
#include <boost/random.hpp>
#endif

namespace leeloo {

template <class RandEng>
struct random
{
	inline random(RandEng& eng):
		_eng(eng)
	{ }

	template <class IntegerType>
	inline IntegerType operator()(IntegerType const a, IntegerType const b)
	{
		return uniform(a, b);
	}

	template <class IntegerType>
	inline IntegerType uniform(IntegerType const a, IntegerType const b)
	{
#ifdef HAS_BOOST_RANDOM
		// Potential support for boost big-ints
		boost::random::uniform_int_distribution<IntegerType> d(a, b);
#else
		std::uniform_int_distribution<IntegerType> d(a, b);
#endif
		return d(_eng);
	}

	template <class IntegerType>
	inline IntegerType uniform()
	{
		return uniform(std::numeric_limits<IntegerType>::min(), std::numeric_limits<IntegerType>::max());
	}

private:
	RandEng& _eng;
};

template <class RandEng>
random<RandEng> random_engine(RandEng& eng)
{
	return random<RandEng>(eng);
}

}

#endif
