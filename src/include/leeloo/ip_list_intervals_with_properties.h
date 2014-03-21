#ifndef LEELOO_IP_LIST_INTERVALS_WITH_PROPERTIES_H
#define LEELOO_IP_LIST_INTERVALS_WITH_PROPERTIES_H

#include <leeloo/ip_list_intervals.h>
#include <leeloo/ips_parser.h>
#include <leeloo/list_intervals_with_properties.h>

namespace leeloo {

template <class Property>
class ip_list_intervals_with_properties: public list_intervals_with_properties<ip_list_intervals, Property>
{
public:
	typedef Property property_type;
	typedef ip_list_intervals::interval_type interval_type;
	typedef ip_list_intervals::base_type base_type;

private:
	typedef list_intervals_with_properties<ip_list_intervals, Property> lip_base_type;

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

	inline void add_property(const char* iprange, property_type const& p)
	{
		ip_list_intervals l;
		ips_parser::parse_ips_add(l, iprange);
		for (ip_interval const& it: const_cast<ip_list_intervals const&>(l).intervals()) {
			this->properties().add_property(it, p);
		}
	}

	property_type const* property_of(const char* ip_str) const
	{
		bool ret = false;
		uint32_t const ip = ips_parser::ipv4toi(ip_str, ret);
		if (!ret) {
			return nullptr;
		}
		return property_of(ip);
	}

	inline property_type const* property_of(uint32_t const ip) const
	{
		return lip_base_type::property_of(ip);
	}
};

}

#endif
