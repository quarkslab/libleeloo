#ifndef LEELOO_IP_LIST_INTERVALS_WITH_PROPERTIES_H
#define LEELOO_IP_LIST_INTERVALS_WITH_PROPERTIES_H

#include <leeloo/ip_list_intervals.h>
#include <leeloo/list_intervals_with_properties.h>

namespace leeloo {

template <class Property>
class ip_list_intervals_with_properties: public list_intervals_with_properties<ip_list_intervals, Property>
{
public:
	typedef Property property_type;
	typedef ip_list_intervals::interval_type interval_type;
	typedef ip_list_intervals::base_type base_type;

public:
	inline void add_property(interval_type const& i, property_type const& p)
	{
		add_property(i.lower(), i.upper(), p);
	}

	inline void add_property(interval_type const& i, property_type&& p)
	{
		add_property(i.lower(), i.upper(), std::move(p));
	}

	inline void add_property(base_type const a, base_type b, property_type const& p)
	{
		if (b != 0xFFFFFFFF) {
			b++;
		}
		this->properties().add_property(a, b, p);
	}

	inline void add_property(base_type const a, base_type b, property_type&& p)
	{
		if (b != 0xFFFFFFFF) {
			b++;
		}
		this->properties().add_property(a, b, std::move(p));
	}
};

}

#endif
