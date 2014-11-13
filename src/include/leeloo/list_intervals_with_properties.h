/* 
 * Copyright (c) 2014, Quarkslab
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

#ifndef LEELOO_LIST_INTERVALS_WITH_PROPERTIES_H
#define LEELOO_LIST_INTERVALS_WITH_PROPERTIES_H

#include <leeloo/uni.h>
#include <leeloo/list_intervals.h>
#include <leeloo/list_intervals_properties.h>

namespace leeloo {

template <class ListInterval, class Property, class PropertiesStorage = PropertiesHorizontal>
class list_intervals_with_properties: public ListInterval
{
public:
	typedef ListInterval list_intervals_type;
	typedef typename list_intervals_type::interval_type interval_type;
	typedef typename list_intervals_type::difference_type difference_type;
	typedef typename list_intervals_type::count_type count_type;
	typedef typename interval_type::base_type base_type;
	typedef Property property_type;
	typedef list_intervals_properties<interval_type, property_type, count_type, PropertiesStorage> list_intervals_properties_type;

public:
	template <template <class T_, bool atomic_> class UPRNG, class Fset, class UPRNGInit>
	void random_sets_with_properties(difference_type size_div, Fset const& fset, UPRNGInit&& uprng_init) const
	{
		// There might be a more efficient way to do this
		if (size_div <= 0) {
			size_div = 1;
		}

		std::vector<property_type const*> properties;
		properties.resize(size_div);

		// AG: 'this' is necessary because the compiler can't know (before
		// instantiation) that random_sets will be part of ListIntervals
		this->template random_sets<UPRNG>(size_div,
			[this, &properties, &fset](base_type* set, count_type size)
			{
				for (count_type i = 0; i < size; i++) {
					properties[i] = property_of(set[i]);
				}
				fset(set, &properties[0], size);
			},
			std::forward<UPRNGInit>(uprng_init));
	}

	/*template <template <class T_, bool atomic_> class UPRNG, class Fsize_div, class Fset, class RandEngine>
	void random_sets_with_properties(Fsize_div const& fsize_div, size_t const size_max, Fset const& fset, RandEngine const& rand_eng) const
	{
		// There might be a more efficient way to do this
		if (size_max <= 0) {
			return;
		}

		std::vector<property_type const*> properties;
		properties.resize(size_max);

		// AG: 'this' is necessary because the compiler can't know (before
		// instantiation) that random_sets will be part of ListIntervals
		this->template random_sets<UPRNG>(fsize_div, size_max,
			[this, &properties, &fset](base_type* set, count_type const size)
			{
				for (count_type i = 0; i < size; i++) {
					properties[i] = property_of(set[i]);
				}
				fset(set, &properties[0], size);
			},
			rand_eng);
	}*/

	template <class Fset, class UPRNGInit>
	inline void random_sets_with_properties(count_type size_div, Fset const& fset, UPRNGInit&& uprng_init) const
	{
		random_sets_with_properties<uni>(size_div, fset, std::forward<UPRNGInit>(uprng_init));
	}

	/*template <class Fsize_div, class Fset, class RandEngine>
	inline void random_sets_with_properties(Fsize_div const& fsize_div, size_t const size_max, Fset const& fset, RandEngine const& rand_eng) const
	{
		random_sets_with_properties<uni>(fsize_div, size_max, fset, rand_eng);
	}*/

	
public:
	// From list_intervals_properties
	
	inline void add_property(interval_type const& i, property_type const& p)
	{
		properties().add_property(i, p);
	}

	inline void add_property(interval_type const& i, property_type&& p)
	{
		properties().add_property(i, std::move(p));
	}

	inline void add_property(base_type const a, base_type const b, property_type const& p)
	{
		properties().add_property(a, b, p);
	}

	inline void add_property(base_type const a, base_type const b, property_type&& p)
	{
		properties().add_property(a, b, std::move(p));
	}

	template <class FAdd, class FRemove>
	inline void aggregate_properties(FAdd const& fadd, FRemove const& fremove)
	{
		properties().aggregate_properties(fadd, fremove);
	}

	template <class FAdd, class FRemove, class FDuplicate>
	inline void aggregate_properties(FAdd const& fadd, FRemove const& fremove, FDuplicate const& fdup)
	{
		properties().aggregate_properties(fadd, fremove, fdup);
	}

	template <class FAdd>
	inline void aggregate_properties_no_rem(FAdd const& fadd)
	{
		properties().aggregate_properties_no_rem(fadd);
	}

	template <class FAdd, class FDuplicate>
	inline void aggregate_properties_no_rem(FAdd const& fadd, FDuplicate const& fdup)
	{
		properties().aggregate_properties_no_rem(fadd, fdup);
	}

	inline property_type const* property_of(base_type const& v) const
	{
		return properties().property_of(v);
	}
	
public:
	inline list_intervals_properties_type& properties()             { return _props; }
	inline list_intervals_properties_type const& properties() const { return _props; }

private:
	list_intervals_properties_type _props;

};

}

#endif
