#ifndef LEELOO_PORT_LIST_INTERVAL_H
#define LEELOO_PORT_LIST_INTERVAL_H

#include <leeloo/exports.h>
#include <leeloo/list_intervals.h>
#include <leeloo/port_interval.h>

namespace leeloo {

class LEELOO_API port_list_intervals: public list_intervals<port_interval>
{
public:
	typedef list_intervals<port_interval> intervals_base_type;

public:
	port_list_intervals():
		intervals_base_type()
	{ }

	port_list_intervals(intervals_base_type const& o):
		intervals_base_type(o)
	{ }

	port_list_intervals(intervals_base_type&& o):
		intervals_base_type(std::move(o))
	{ }

public:
	inline void add(uint16_t a, uint16_t b, port::protocol_enum protocol)
	{
		intervals_base_type::add(port(a, protocol), port(b, protocol).as_u32()+1);
	}

	inline void add(port const p)
	{
		intervals_base_type::add(p, p.as_u32()+1);
	}

	inline void remove(uint16_t a, uint16_t b, port::protocol_enum protocol)
	{
		intervals_base_type::remove(port(a, protocol), port(b, protocol).as_u32()+1);
	}

	inline void remove(port const p)
	{
		intervals_base_type::remove(p, p.as_u32()+1);
	}

	inline bool contains(port const p)
	{
		return intervals_base_type::contains(p);
	}

	inline void intersect(port_list_intervals const& o)
	{
		intervals_base_type::intersect(o);
	}

private:
	// With our actual representation, this does not make any sense
	inline port_list_intervals invert() const
	{
		return intervals_base_type::invert();
	}
};

}

#endif
