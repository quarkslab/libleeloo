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
	typedef typename list_intervals_type::size_type size_type;
	typedef typename interval_type::base_type base_type;
	typedef Property property_type;
	typedef list_intervals_properties<interval_type, property_type, size_type, PropertiesStorage> list_intervals_properties_type;

public:
	template <template <class T_, bool atomic_> class UPRNG, class Fset, class RandEngine>
	void random_sets_with_properties(size_type size_div, Fset const& fset, RandEngine const& rand_eng) const
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
			[this, &properties, &fset](base_type* set, size_type size)
			{
				for (size_type i = 0; i < size; i++) {
					properties[i] = property_of(set[i]);
				}
				fset(set, &properties[0], size);
			},
			rand_eng);
	}

	template <class Fset, class RandEngine>
	inline void random_sets_with_properties(size_type size_div, Fset const& fset, RandEngine const& rand_eng) const
	{
		random_sets_with_properties<uni>(size_div, fset, rand_eng);
	}

	
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
