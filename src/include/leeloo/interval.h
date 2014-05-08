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

#ifndef LEELOO_INTERVAL_H
#define LEELOO_INTERVAL_H

#include <algorithm>

#include <leeloo/exports.h>

#ifdef LEELOO_BOOST_SERIALIZE
#include <boost/serialization/version.hpp>
#endif

namespace leeloo {

// interval is half-opened [lower,upper[
template <class Integer>
class interval
{
public:
	typedef Integer base_type;

public:
	interval()
	{ }

	interval(base_type const lower, base_type const upper):
		_lower(lower),
		_upper(upper)
	{ }

	interval(interval const& o):
		_lower(o.lower()),
		_upper(o.upper())
	{ }

public:
	inline base_type lower() const  { return _lower; }
	inline base_type upper() const  { return _upper; }
	inline base_type middle() const { return (upper()+lower())/2; }

	inline void assign(base_type const lower, base_type const upper)
	{
		_lower = lower;
		_upper = upper;
	}

	inline void set_lower(base_type const lower) { _lower = lower; }
	inline void set_upper(base_type const upper) { _upper = upper; }

	inline base_type width() const { return upper()-lower(); }

	inline interval& operator=(interval const& o)
	{
		// Don't care about self assignment
		_lower = o._lower;
		_upper = o._upper;
		return *this;
	}

	inline bool contains(base_type const v) const
	{
		return (v >= _lower) && (v < _upper);
	}

#ifdef LEELOO_BOOST_SERIALIZE
	template<class Archive>
	void serialize(Archive& ar, unsigned int const /*version*/)
	{
		ar & _lower;
		ar & _upper;
	}
#endif

public:
	static interval empty()
	{
		return interval(base_type(0), base_type(0));
	}

private:
	base_type _lower;
	base_type _upper;
};

} // leeloo

// Common exported instanciations (outside of any namespace)

#ifdef LEELOO_INCLUDE_U8
LEELOO_TEMPLATE_EXPIMP template class LEELOO_API leeloo::interval<uint8_t>;
#endif

#ifdef LEELOO_INCLUDE_U16
LEELOO_TEMPLATE_EXPIMP template class LEELOO_API leeloo::interval<uint16_t>;
#endif

#ifdef LEELOO_INCLUDE_U32
LEELOO_TEMPLATE_EXPIMP template class LEELOO_API leeloo::interval<uint32_t>;
#endif

#ifdef LEELOO_INCLUDE_U64
LEELOO_TEMPLATE_EXPIMP template class LEELOO_API leeloo::interval<uint64_t>;
#endif

#endif
