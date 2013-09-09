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

#ifndef LEELO_IP_LIST_INTERVALS_H
#define LEELO_IP_LIST_INTERVALS_H

#include <cstdint>
#include <leeloo/interval.h>
#include <leeloo/list_intervals.h>

namespace leeloo {

typedef interval<uint32_t> ip_interval;

class ip_list_intervals: public list_intervals<ip_interval>
{
	typedef list_intervals<ip_interval> interval_base_type;

public:
	bool add(const char* str_interval);
	bool remove(const char* str_interval);

	template <bool exclude = false>
	inline void insert(const char* str_interval)
	{
		if (exclude) {
			remove(str_interval);
		}
		else {
			add(str_interval);
		}
	}

	// Interval here is closed ([a,b]). This will force the non-inclusion of 255.255.255.255.
	inline void add(uint32_t const a, uint32_t const b)
	{
		uint32_t b_ = b;
		if (b_ != 0xFFFFFFFF) {
			b_++;
		}
		interval_base_type::add(a, b_);
	}

	inline void add(uint32_t const a)
	{
		if (a != 0xFFFFFFFF) {
			interval_base_type::add(a, a+1);
		}
	}

	inline void remove(uint32_t const a, uint32_t const b)
	{
		uint32_t b_ = b;
		if (b_ != 0xFFFFFFFF) {
			b_++;
		}
		interval_base_type::remove(a, b_);
	}

	inline void remove(uint32_t const a)
	{
		if (a != 0xFFFFFFFF) {
			interval_base_type::remove(a, a+1);
		}
	}

	template <bool exclude = false>
	inline void insert(uint32_t const a)
	{
		if (exclude) {
			remove(a);
		}
		else {
			add(a);
		}
	}

	template <bool exclude = false>
	inline void insert(uint32_t const a, uint32_t const b)
	{
		if (exclude) {
			remove(a, b);
		}
		else {
			add(a, b);
		}
	}

	inline bool contains(uint32_t const ip) const { return interval_base_type::contains(ip); }

	bool contains(const char* ip_str) const;
};

}

#endif
