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

#ifndef LEELO_IPV6_LIST_INTERVALS_H
#define LEELO_IPV6_LIST_INTERVALS_H

#include <leeloo/exports.h>
#include <leeloo/list_intervals.h>
#include <leeloo/ipv6.h>

#include <cstdint>

namespace leeloo {

class LEELOO_API ipv6_list_intervals: public list_intervals<ipv6_interval>
{
	typedef list_intervals<ipv6_interval> intervals_base_type;

public:
	ipv6_list_intervals():
		intervals_base_type()
	{ }

	ipv6_list_intervals(intervals_base_type const& o):
		intervals_base_type(o)
	{ }

	ipv6_list_intervals(intervals_base_type&& o):
		intervals_base_type(std::move(o))
	{ }

public:
	bool add(const char* str_interval);
	bool remove(const char* str_interval);

	template <bool exclude = false>
	inline void insert(typename std::enable_if<exclude == true, const char*>::type str_interval)
	{
		remove(str_interval);
	}

	template <bool exclude = false>
	inline void insert(typename std::enable_if<exclude == false, const char*>::type str_interval)
	{
		add(str_interval);
	}

	// Interval here is closed ([a,b])
	inline void add(ipv6_int const& a, ipv6_int const& b)
	{
		ipv6_int b_ = b;
		if (b_ != ipv6_max_int) {
			b_++;
		}
		intervals_base_type::add(a, b_);
	}

	inline void add(ipv6_int const& a)
	{
		if (a != ipv6_max_int) {
			intervals_base_type::add(a, a+1);
		}
	}

	inline void add(leeloo::ipv6_list_intervals const& o)
	{
		intervals_base_type::add(o);
	}

	inline void remove(ipv6_int const& a, ipv6_int const& b)
	{
		ipv6_int b_ = b;
		if (b_ != ipv6_max_int) {
			b_++;
		}
		intervals_base_type::remove(a, b_);
	}

	inline void remove(ipv6_int const& a)
	{
		if (a != ipv6_max_int) {
			intervals_base_type::remove(a, a+1);
		}
	}

	inline void remove(leeloo::ipv6_list_intervals const& o)
	{
		intervals_base_type::remove(o);
	}

	template <bool exclude = false>
	inline void insert(ipv6_int const& a)
	{
		if (exclude) {
			remove(a);
		}
		else {
			add(a);
		}
	}

	template <bool exclude = false>
	inline void insert(ipv6_int const& a, ipv6_int const& b)
	{
		if (exclude) {
			remove(a, b);
		}
		else {
			add(a, b);
		}
	}

	template <bool exclude = false>
	inline void insert_prefix(ipv6_int const& base_ip, const int prefix)
	{
		assert(prefix >= 0);
		const ipv6_int mask = prefix2mask(prefix);
		insert<exclude>(base_ip & (~mask), base_ip|mask);
	}

	using intervals_base_type::contains;
	bool contains(const char* ipv6_str) const;

	inline ipv6_list_intervals invert() const
	{
		return intervals_base_type::invert();
	}

	inline void intersect(ipv6_list_intervals const& o)
	{
		intervals_base_type::intersect(o);
	}

	static inline ipv6_int prefix2mask(const int prefix)
	{
		assert(prefix >= 0);
		if (prefix == 0) {
			return 0;
		}
		return (ipv6_int(1)<<(128-prefix))-1;
	}
};

}

#endif
