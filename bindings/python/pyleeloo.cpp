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

#include <boost/random.hpp>
#include <boost/python.hpp>

#include <cstdint>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>

#include <leeloo/config.h>
#include <leeloo/ip_list_intervals.h>
#include <leeloo/ip_list_intervals_with_properties.h>
#include <leeloo/ips_parser.h>
#include <leeloo/list_intervals.h>
#include <leeloo/list_intervals_random.h>
#include <leeloo/port_list_intervals.h>
#include <leeloo/random.h>
#include <leeloo/uni.h>

#ifdef LEELOO_BOOST_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#endif

using namespace boost::python;

boost::random::mt19937 g_mt_rand;

void (leeloo::ip_list_intervals::*ip_add1)(leeloo::ip_list_intervals::base_type const, leeloo::ip_list_intervals::base_type const) = &leeloo::ip_list_intervals::add;
void (leeloo::ip_list_intervals::*ip_add2)(leeloo::ip_list_intervals::base_type const)                                             = &leeloo::ip_list_intervals::add;
bool (leeloo::ip_list_intervals::*ip_add3)(const char*)                                                                            = &leeloo::ip_list_intervals::add;
void (leeloo::ip_list_intervals::*ip_add4)(leeloo::ip_list_intervals const&)                                                       = &leeloo::ip_list_intervals::add;

void (leeloo::ip_list_intervals::*ip_remove1)(leeloo::ip_list_intervals::base_type const, leeloo::ip_list_intervals::base_type const) = &leeloo::ip_list_intervals::remove;
void (leeloo::ip_list_intervals::*ip_remove2)(leeloo::ip_list_intervals::base_type const)                                             = &leeloo::ip_list_intervals::remove;
bool (leeloo::ip_list_intervals::*ip_remove3)(const char*)                                                                            = &leeloo::ip_list_intervals::remove;
void (leeloo::ip_list_intervals::*ip_remove4)(leeloo::ip_list_intervals const&)                                                       = &leeloo::ip_list_intervals::remove;

bool (leeloo::ip_list_intervals::*contains1)(uint32_t const) const = &leeloo::ip_list_intervals::contains;
bool (leeloo::ip_list_intervals::*contains2)(const char*)    const = &leeloo::ip_list_intervals::contains;

void (leeloo::port_list_intervals::*port_add1)(uint16_t const, uint16_t const, leeloo::port::protocol_enum) = &leeloo::port_list_intervals::add;
void (leeloo::port_list_intervals::*port_add2)(leeloo::port const)                                          = &leeloo::port_list_intervals::add;

void (leeloo::port_list_intervals::*port_remove1)(uint16_t const, uint16_t const, leeloo::port::protocol_enum) = &leeloo::port_list_intervals::remove;
void (leeloo::port_list_intervals::*port_remove2)(leeloo::port const)                                          = &leeloo::port_list_intervals::remove;

typedef uint16_t (leeloo::port::*get_port_const)() const;

template <class Integer>
class set_read_only
{
public:
	typedef Integer const* iterator;

public:
	set_read_only():
		_buf(nullptr),
		_size(0)
	{ }

	set_read_only(Integer const* buf, size_t const size):
		_buf(buf),
		_size(size)
	{ }

public:
	inline Integer at(size_t idx) const
	{
		assert(idx < _size);
		return _buf[idx];
	}

	inline size_t size() const { return _size; }

	Integer const* begin() const { return _buf; }
	Integer const* end() const   { return _buf+_size; }

private:
	Integer const* _buf;
	size_t _size;
};

typedef set_read_only<uint16_t> u16_set_read_only;
typedef set_read_only<uint32_t> u32_set_read_only;

static void ip_list_random_sets(leeloo::ip_list_intervals const& l, size_t const size_div, object& f_set)
{
	l.random_sets(size_div,
	              [&f_set](uint32_t const* buf, size_t const size) { f_set(u32_set_read_only(buf, size)); },
				  leeloo::random_engine<uint32_t>(g_mt_rand));
}

static void ip_list_random_sets_func(leeloo::ip_list_intervals const& l, object& f_size_div, size_t const size_max, object& f_set)
{
	l.random_sets(
			[&f_size_div](size_t const i) -> size_t
			{
				return extract<size_t>(f_size_div(i));
			},
		    size_max,
		    [&f_set](uint32_t const* buf, size_t const size) { f_set(u32_set_read_only(buf, size)); },
		    leeloo::random_engine<uint32_t>(g_mt_rand));
}

static void port_list_random_sets(leeloo::port_list_intervals const& l, size_t const size_div, object& f_set)
{
	l.random_sets(size_div,
	              [&f_set](uint32_t const* buf, size_t const size) { f_set(u32_set_read_only(buf, size)); },
				  leeloo::random_engine<uint32_t>(g_mt_rand));
}

static void init_rand_gen()
{
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd != -1) {
		uint32_t seed;
		read(fd, &seed, sizeof(uint32_t));
		g_mt_rand.seed(seed);
		close(fd);
	}
	else {
		g_mt_rand.seed(time(NULL));
	}
}

static uint32_t python_ipv4toi1(const char* ip)
{
	bool valid;
	return leeloo::ips_parser::ipv4toi(ip, strlen(ip), valid);
}

static uint32_t python_ipv4toi2(const char* ip, bool& valid)
{
	return leeloo::ips_parser::ipv4toi(ip, strlen(ip), valid);
}

// uint16 intervals
//

typedef leeloo::interval<uint16_t> u16_interval;
typedef leeloo::list_intervals<u16_interval> u16_list_intervals;

void (u16_list_intervals::*u16_add1)(u16_list_intervals::base_type const, u16_list_intervals::base_type const) = &u16_list_intervals::add;
void (u16_list_intervals::*u16_add2)(u16_list_intervals const&)                                                = &u16_list_intervals::add;

static void u16_list_random_sets(u16_list_intervals const& l, size_t const size_div, object& f_set)
{
	l.random_sets(size_div,
	              [&f_set](uint16_t const* buf, size_t const size) { f_set(u16_set_read_only(buf, size)); },
				  leeloo::random_engine<uint16_t>(g_mt_rand));
}

static void u16_list_random_sets_func(u16_list_intervals const& l, object& f_size_div, size_t const size_max, object& f_set)
{
	l.random_sets(
			[&f_size_div](size_t const i) -> size_t
			{
				return extract<size_t>(f_size_div(i));
			},
	        size_max,
	        [&f_set](uint16_t const* buf, size_t const size) { f_set(u16_set_read_only(buf, size)); },
			leeloo::random_engine<uint16_t>(g_mt_rand));
}

// uint32 intervals
//

typedef leeloo::interval<uint32_t> u32_interval;
typedef leeloo::list_intervals<u32_interval> u32_list_intervals;

void (u32_list_intervals::*u32_add1)(u32_list_intervals::base_type const, u32_list_intervals::base_type const) = &u32_list_intervals::add;
void (u32_list_intervals::*u32_add2)(u32_list_intervals const&)                                                = &u32_list_intervals::add;

static void u32_list_random_sets(u32_list_intervals const& l, size_t const size_div, object& f_set)
{
	l.random_sets(size_div,
	              [&f_set](uint32_t const* buf, size_t const size) { f_set(u32_set_read_only(buf, size)); },
				  leeloo::random_engine<uint32_t>(g_mt_rand));
}

// Properties
typedef object property_python;

template <class Integer, class Property>
class set_properties_read_only
{
public:
	typedef Integer const* iterator;

public:
	set_properties_read_only():
		_buf(nullptr),
		_properties(nullptr),
		_size(0)
	{ }

	set_properties_read_only(Integer const* buf, Property const* const* properties, size_t const size):
		_buf(buf),
		_properties(properties),
		_size(size)
	{ }

public:
	inline Integer at(size_t idx) const
	{
		assert(idx < _size);
		return _buf[idx];
	}

	inline Property const* property_at(size_t idx) const
	{
		assert(idx < _size);
		return _properties[idx];
	}

	inline size_t size() const { return _size; }

private:
	Integer const* _buf;
	Property const* const* _properties;
	size_t _size;
};

// IP intervals with properties
typedef leeloo::ip_list_intervals_with_properties<property_python> ip_list_intervals_with_properties_python;
typedef set_properties_read_only<uint32_t, property_python> u32_set_properties_read_only_python;

void (ip_list_intervals_with_properties_python::*ip_add_property1)(uint32_t const, uint32_t, property_python const& p)  = &ip_list_intervals_with_properties_python::add_property;
void (ip_list_intervals_with_properties_python::*ip_add_property2)(leeloo::ip_interval const&, property_python const&p) = &ip_list_intervals_with_properties_python::add_property;
void (ip_list_intervals_with_properties_python::*ip_add_property3)(const char*, property_python const&p)                = &ip_list_intervals_with_properties_python::add_property;

static property_python u32_set_properties_read_only_python_property_at_wrapper(u32_set_properties_read_only_python const& s, size_t idx)
{
	property_python const* p = s.property_at(idx);
	if (p == nullptr) {
		return property_python();
	}
	return *p;
}

static void ip_list_intervals_with_properties_python_aggregate_properties(ip_list_intervals_with_properties_python& l, object& fadd, object& fremove)
{
	const object copyMod = import("copy");
	const object deepcopy = copyMod.attr("deepcopy");
	l.aggregate_properties(fadd, fremove,
		[&deepcopy](property_python const& p)
		{
			return deepcopy(p);
		});
}

static void ip_list_intervals_with_properties_python_aggregate_properties_no_rem(ip_list_intervals_with_properties_python& l, object& fadd)
{
	const object copyMod = import("copy");
	const object deepcopy = copyMod.attr("deepcopy");
	l.aggregate_properties_no_rem(fadd,
		[&deepcopy](property_python const& p)
		{
			return deepcopy(p);
		});
}

static void ip_list_with_properties_python_random_sets_with_properties(ip_list_intervals_with_properties_python const& l, size_t const size_div, object& f_set)
{
	l.random_sets_with_properties(size_div, 
		[&f_set](uint32_t const* buf, property_python const* const* props, size_t const size)
		{
			f_set(u32_set_properties_read_only_python(buf, props, size));
		}
		,
		leeloo::random_engine<uint64_t>(g_mt_rand));
}

static void ip_list_with_properties_python_random_sets_with_properties_func(ip_list_intervals_with_properties_python const& l, object& f_size_div, size_t const size_max, object& f_set)
{
	l.random_sets_with_properties(
		[&f_size_div](size_t const i) -> size_t
		{
			return extract<size_t>(f_size_div(i));
		},
		size_max,
		[&f_set](uint32_t const* buf, property_python const* const* props, size_t const size)
		{
			f_set(u32_set_properties_read_only_python(buf, props, size));
		}
		,
		leeloo::random_engine<uint32_t>(g_mt_rand));
}

static property_python ip_list_intervals_with_properties_python_property_of_wrapper(ip_list_intervals_with_properties_python const& l, uint32_t v)
{
	property_python const* p = l.property_of(v);
	if (p == nullptr) {
		return property_python();
	}
	return *p;
}

static property_python ip_list_intervals_with_properties_python_property_of_wrapper_str(ip_list_intervals_with_properties_python const& l, const char* str)
{
	property_python const* p = l.property_of(str);
	if (p == nullptr) {
		return property_python();
	}
	return *p;
}

std::string port_repr(leeloo::port const& port)
{
	std::stringstream ss;
	ss << "<Port " << port.value() << "/" << leeloo::port::protocol_name(port.protocol()) << ">";
	return ss.str();
}

uint32_t port_hash(leeloo::port const& port)
{
	return port.as_u32();
}

typedef leeloo::list_intervals_random<leeloo::ip_list_intervals, leeloo::uni> ip_list_intervals_random;
typedef leeloo::list_intervals_random<u32_list_intervals, leeloo::uni> u32_list_intervals_random;

void ip_list_intervals_random_init(ip_list_intervals_random& ipr, leeloo::ip_list_intervals const& ipl)
{
	ipr.init(ipl, leeloo::random_engine<uint32_t>(g_mt_rand));
}

void ip_list_intervals_random_init_seed(ip_list_intervals_random& ipr, leeloo::ip_list_intervals const& ipl, ip_list_intervals_random::seed_type const seed, leeloo::ip_list_intervals::size_type const start)
{
	ipr.init(ipl, leeloo::random_engine<uint32_t>(g_mt_rand), seed, start);
}

void u32_list_intervals_random_init(u32_list_intervals_random& u32r, u32_list_intervals const& u32l)
{
	u32r.init(u32l, leeloo::random_engine<uint32_t>(g_mt_rand));
}

void u32_list_intervals_random_init_seed(u32_list_intervals_random& u32r, u32_list_intervals const& u32l, u32_list_intervals_random::seed_type const seed, u32_list_intervals::size_type const start)
{
	u32r.init(u32l, leeloo::random_engine<uint32_t>(g_mt_rand), seed, start);
}


typedef leeloo::list_intervals_random_promise<leeloo::ip_list_intervals, leeloo::uni> ip_list_intervals_random_promise;

void ip_list_intervals_random_promise_init(ip_list_intervals_random_promise& ipr, leeloo::ip_list_intervals const& ipl)
{
	ipr.init(ipl, leeloo::random_engine<uint32_t>(g_mt_rand));
}

void ip_list_intervals_random_promise_init_seed(ip_list_intervals_random_promise& ipr, leeloo::ip_list_intervals const& ipl, ip_list_intervals_random_promise::seed_type const seed)
{
	ipr.init(ipl, leeloo::random_engine<uint32_t>(g_mt_rand), seed);
}

void ip_list_intervals_random_promise_init_seed_steps(ip_list_intervals_random_promise& ipr, leeloo::ip_list_intervals const& ipl, ip_list_intervals_random_promise::seed_type const seed, ip_list_intervals_random_promise::size_type step_start, ip_list_intervals_random_promise::size_type step_end)
{
	ipr.init(ipl, leeloo::random_engine<uint32_t>(g_mt_rand), seed, step_start, step_end);
}

#ifdef LEELOO_BOOST_SERIALIZE
void ip_list_intervals_random_promise_save_state(ip_list_intervals_random_promise& ipr, const char* file)
{
	std::ofstream ofs(file, std::ofstream::out | std::ofstream::trunc);
	boost::archive::text_oarchive oa(ofs);
	ipr.save_state(oa);
}

void ip_list_intervals_random_promise_restore_state(ip_list_intervals_random_promise& ipr, const char* file, leeloo::ip_list_intervals const& ipl)
{
	std::ifstream ifs(file, std::ifstream::in);
	boost::archive::text_iarchive ia(ifs);
	ipr.restore_state(ia, ipl, leeloo::random_engine<uint32_t>(g_mt_rand));
}
#endif

static leeloo::port tcp_port(uint16_t value)
{
	return leeloo::port(value, leeloo::port::protocol_enum::TCP);
}

static leeloo::port udp_port(uint16_t value)
{
	return leeloo::port(value, leeloo::port::protocol_enum::UDP);
}

static leeloo::port sctp_port(uint16_t value)
{
	return leeloo::port(value, leeloo::port::protocol_enum::SCTP);
}

BOOST_PYTHON_MODULE(pyleeloo)
{
	init_rand_gen();

	enum_<leeloo::port::protocol_enum>("protocol")
		.value("TCP", leeloo::port::protocol_enum::TCP)
		.value("UDP", leeloo::port::protocol_enum::UDP)
		.value("SCTP", leeloo::port::protocol_enum::SCTP)
		;

	class_<leeloo::ip_interval>("ip_interval")
		.def("assign", &leeloo::ip_interval::assign)
		.def("lower", &leeloo::ip_interval::lower)
		.def("upper", &leeloo::ip_interval::upper)
		.def("set_lower", &leeloo::ip_interval::set_lower)
		.def("set_upper", &leeloo::ip_interval::set_upper)
		;

	class_<u16_interval>("u16_interval")
		.def("assign", &u16_interval::assign)
		.def("lower", &u16_interval::lower)
		.def("upper", &u16_interval::upper)
		.def("set_lower", &u16_interval::set_lower)
		.def("set_upper", &u16_interval::set_upper)
		;

	class_<leeloo::port>("port")
		.def(init<uint16_t, leeloo::port::protocol_enum>())
		.def(init<uint32_t>())
		.def("value", get_port_const(&leeloo::port::value))
		.def("protocol", get_port_const(&leeloo::port::protocol))
		.def("as_u32", &leeloo::port::as_u32)
		.def("__repr__", &port_repr)
		.def("__hash__", &port_hash)
		.def("__eq__", &leeloo::port::operator==)
		;

	class_<leeloo::port_interval>("port_interval")
		.def("port_lower", &leeloo::port_interval::port_lower)
		.def("port_upper", &leeloo::port_interval::port_upper)
		;

	class_<leeloo::ip_list_intervals>("ip_list_intervals")
		.def("add", ip_add1)
		.def("add", ip_add2)
		.def("add", ip_add3)
		.def("add", ip_add4)
		.def("remove", ip_remove1)
		.def("remove", ip_remove2)
		.def("remove", ip_remove3)
		.def("remove", ip_remove4)
		.def("aggregate", &leeloo::ip_list_intervals::aggregate)
		.def("aggregate_max_prefix", &ip_list_intervals_with_properties_python::aggregate_max_prefix)
		.def("create_index_cache", &leeloo::ip_list_intervals::create_index_cache)
		.def("size", &leeloo::ip_list_intervals::size)
		.def("intervals_count", &leeloo::ip_list_intervals::intervals_count)
		.def("reserve", &leeloo::ip_list_intervals::reserve)
		.def("clear", &leeloo::ip_list_intervals::clear)
		.def("at", &leeloo::ip_list_intervals::at)
		.def("at_cached", &leeloo::ip_list_intervals::at_cached)
		.def("random_sets", &ip_list_random_sets)
		.def("random_sets", &ip_list_random_sets_func)
		.def("contains", contains1)
		.def("contains", contains2)
		.def("dump_to_file", &leeloo::ip_list_intervals::dump_to_file)
		.def("read_from_file", &leeloo::ip_list_intervals::read_from_file)
		.def("__iter__", iterator<leeloo::ip_list_intervals>())
		;

	class_<u16_list_intervals>("u16_list_intervals")
		.def("add", u16_add1)
		.def("add", u16_add2)
		.def("aggregate", &u16_list_intervals::aggregate)
		.def("aggregate_max_prefix", &ip_list_intervals_with_properties_python::aggregate_max_prefix)
		.def("create_index_cache", &u16_list_intervals::create_index_cache)
		.def("size", &u16_list_intervals::size)
		.def("intervals_count", &u16_list_intervals::intervals_count)
		.def("reserve", &u16_list_intervals::reserve)
		.def("clear", &u16_list_intervals::clear)
		.def("at", &u16_list_intervals::at)
		.def("at_cached", &u16_list_intervals::at_cached)
		.def("random_sets", &u16_list_random_sets)
		.def("random_sets", &u16_list_random_sets_func)
		.def("dump_to_file", &u16_list_intervals::dump_to_file)
		.def("read_from_file", &u16_list_intervals::read_from_file)
		.def("contains", &u16_list_intervals::contains)
		.def("__iter__", iterator<u16_list_intervals>())
		;

	class_<u32_list_intervals>("u32_list_intervals")
		.def("add", u32_add1)
		.def("add", u32_add2)
		.def("aggregate", &u32_list_intervals::aggregate)
		.def("aggregate_max_prefix", &ip_list_intervals_with_properties_python::aggregate_max_prefix)
		.def("create_index_cache", &u32_list_intervals::create_index_cache)
		.def("size", &u32_list_intervals::size)
		.def("intervals_count", &u32_list_intervals::intervals_count)
		.def("reserve", &u32_list_intervals::reserve)
		.def("clear", &u32_list_intervals::clear)
		.def("at", &u32_list_intervals::at)
		.def("at_cached", &u32_list_intervals::at_cached)
		.def("random_sets", &u32_list_random_sets)
		.def("dump_to_file", &u32_list_intervals::dump_to_file)
		.def("read_from_file", &u32_list_intervals::read_from_file)
		.def("contains", &u32_list_intervals::contains)
		.def("__iter__", iterator<u32_list_intervals>())
		;

	class_<ip_list_intervals_with_properties_python>("ip_list_intervals_with_properties")
		.def("add", ip_add1)
		.def("add", ip_add2)
		.def("add", ip_add3)
		.def("add", ip_add4)
		.def("remove", ip_remove1)
		.def("remove", ip_remove2)
		.def("remove", ip_remove3)
		.def("remove", ip_remove4)
		.def("add_property", ip_add_property1)
		.def("add_property", ip_add_property2)
		.def("add_property", ip_add_property3)
		.def("aggregate", &ip_list_intervals_with_properties_python::aggregate)
		.def("aggregate_max_prefix", &ip_list_intervals_with_properties_python::aggregate_max_prefix)
		.def("aggregate_properties", &ip_list_intervals_with_properties_python_aggregate_properties)
		.def("aggregate_properties_no_rem", &ip_list_intervals_with_properties_python_aggregate_properties_no_rem)
		.def("property_of", &ip_list_intervals_with_properties_python_property_of_wrapper)
		.def("property_of", &ip_list_intervals_with_properties_python_property_of_wrapper_str)
		.def("create_index_cache", &ip_list_intervals_with_properties_python::create_index_cache)
		.def("size", &ip_list_intervals_with_properties_python::size)
		.def("intervals_count", &ip_list_intervals_with_properties_python::intervals_count)
		.def("reserve", &ip_list_intervals_with_properties_python::reserve)
		.def("clear", &ip_list_intervals_with_properties_python::clear)
		.def("at", &ip_list_intervals_with_properties_python::at)
		.def("at_cached", &ip_list_intervals_with_properties_python::at_cached)
		.def("random_sets", &ip_list_random_sets)
		.def("random_sets", &ip_list_random_sets_func)
		.def("random_sets_with_properties", &ip_list_with_properties_python_random_sets_with_properties)
		.def("random_sets_with_properties", &ip_list_with_properties_python_random_sets_with_properties_func)
		.def("contains", contains1)
		.def("contains", contains2)
		.def("dump_to_file", &ip_list_intervals_with_properties_python::dump_to_file)
		.def("read_from_file", &ip_list_intervals_with_properties_python::read_from_file)
		.def("__iter__", iterator<ip_list_intervals_with_properties_python>())
		;

	class_<leeloo::port_list_intervals>("port_list_intervals")
		.def("add", port_add1)
		.def("add", port_add2)
		.def("remove", port_remove1)
		.def("remove", port_remove2)
		.def("aggregate", &leeloo::port_list_intervals::aggregate)
		.def("create_index_cache", &leeloo::port_list_intervals::create_index_cache)
		.def("size", &leeloo::port_list_intervals::size)
		.def("intervals_count", &leeloo::port_list_intervals::intervals_count)
		.def("reserve", &leeloo::port_list_intervals::reserve)
		.def("clear", &leeloo::port_list_intervals::clear)
		.def("at", &leeloo::port_list_intervals::at)
		.def("random_sets", &port_list_random_sets)
		.def("contains", &leeloo::port_list_intervals::contains)
		.def("dump_to_file", &leeloo::port_list_intervals::dump_to_file)
		.def("read_from_file", &leeloo::port_list_intervals::read_from_file)
		.def("__iter__", iterator<leeloo::port_list_intervals>())
		;

	class_<u16_set_read_only>("u16_set_read_only")
		.def("at", &u16_set_read_only::at)
		.def("size", &u16_set_read_only::size)
		.def("__iter__", iterator<u16_set_read_only>())
		;

	class_<u32_set_read_only>("u32_set_read_only")
		.def("at", &u32_set_read_only::at)
		.def("size", &u32_set_read_only::size)
		.def("__iter__", iterator<u32_set_read_only>())
		;

	class_<u32_set_properties_read_only_python>("u32_set_properties_read_only_python")
		.def("at", &u32_set_properties_read_only_python::at)
		.def("property_at", &u32_set_properties_read_only_python_property_at_wrapper)
		.def("size", &u32_set_properties_read_only_python::size)
		;

	class_<u32_list_intervals_random>("u32_list_intervals_random")
		.def("init", &u32_list_intervals_random_init)
		.def("init", &u32_list_intervals_random_init_seed)
		.def("__call__", &u32_list_intervals_random::operator())
		.def("end", &u32_list_intervals_random::end)
		.def("cur_step", &u32_list_intervals_random::cur_step)
		;

	class_<ip_list_intervals_random>("ip_list_intervals_random")
		.def("init", &ip_list_intervals_random_init)
		.def("init", &ip_list_intervals_random_init_seed)
		.def("__call__", &ip_list_intervals_random::operator())
		.def("end", &ip_list_intervals_random::end)
		.def("cur_step", &ip_list_intervals_random::cur_step)
		;

	class_<ip_list_intervals_random_promise>("ip_list_intervals_random_promise")
		.def("init", &ip_list_intervals_random_promise_init)
		.def("init", &ip_list_intervals_random_promise_init_seed)
		.def("init", &ip_list_intervals_random_promise_init_seed_steps)
		.def("__call__", &ip_list_intervals_random_promise::operator())
		.def("end", &ip_list_intervals_random_promise::end)
		.def("get_current_step", &ip_list_intervals_random_promise::get_current_step)
		.def("step_done", &ip_list_intervals_random_promise::step_done)
		.def("size_todo", &ip_list_intervals_random_promise::size_todo)
#ifdef LEELOO_BOOST_SERIALIZE
		.def("save_state", &ip_list_intervals_random_promise_save_state)
		.def("restore_state", &ip_list_intervals_random_promise_restore_state)
#endif
		;

	def("ipv4toi", python_ipv4toi1);
	def("ipv4toi", python_ipv4toi2);

	def("tcp_port", tcp_port);
	def("udp_port", udp_port);
	def("sctp_port", sctp_port);
}
