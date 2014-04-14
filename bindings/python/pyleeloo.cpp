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

#include <leeloo/ip_list_intervals.h>
#include <leeloo/ip_list_intervals_with_properties.h>
#include <leeloo/ips_parser.h>
#include <leeloo/list_intervals.h>
#include <leeloo/random.h>

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
typedef set_read_only<uint64_t> u64_set_read_only;

static void ip_list_random_sets(leeloo::ip_list_intervals const& l, size_t const size_div, object& f_set)
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

// uint64 intervals
//

typedef leeloo::interval<uint64_t> u64_interval;
typedef leeloo::list_intervals<u64_interval> u64_list_intervals;

void (u64_list_intervals::*u64_add1)(u64_list_intervals::base_type const, u64_list_intervals::base_type const) = &u64_list_intervals::add;
void (u64_list_intervals::*u64_add2)(u64_list_intervals const&)                                                = &u64_list_intervals::add;

static void u64_list_random_sets(u64_list_intervals const& l, size_t const size_div, object& f_set)
{
	l.random_sets(size_div,
	              [&f_set](uint64_t const* buf, size_t const size) { f_set(u64_set_read_only(buf, size)); },
				  leeloo::random_engine<uint64_t>(g_mt_rand));
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

BOOST_PYTHON_MODULE(pyleeloo)
{
	init_rand_gen();

	class_<leeloo::ip_interval>("ip_interval")
		.def("assign", &leeloo::ip_interval::assign)
		.def("lower", &leeloo::ip_interval::lower)
		.def("upper", &leeloo::ip_interval::upper)
		.def("set_lower", &leeloo::ip_interval::set_lower)
		.def("set_upper", &leeloo::ip_interval::set_upper);

	class_<u16_interval>("u16_interval")
		.def("assign", &u16_interval::assign)
		.def("lower", &u16_interval::lower)
		.def("upper", &u16_interval::upper)
		.def("set_lower", &u16_interval::set_lower)
		.def("set_upper", &u16_interval::set_upper);

	class_<u64_interval>("u64_interval")
		.def("assign", &u64_interval::assign)
		.def("lower", &u64_interval::lower)
		.def("upper", &u64_interval::upper)
		.def("set_lower", &u64_interval::set_lower)
		.def("set_upper", &u64_interval::set_upper);

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
		.def("create_index_cache", &leeloo::ip_list_intervals::create_index_cache)
		.def("size", &leeloo::ip_list_intervals::size)
		.def("reserve", &leeloo::ip_list_intervals::reserve)
		.def("clear", &leeloo::ip_list_intervals::clear)
		.def("at", &leeloo::ip_list_intervals::at)
		.def("random_sets", &ip_list_random_sets)
		.def("contains", contains1)
		.def("contains", contains2)
		.def("dump_to_file", &leeloo::ip_list_intervals::dump_to_file)
		.def("read_from_file", &leeloo::ip_list_intervals::read_from_file)
		.def("__iter__", iterator<leeloo::ip_list_intervals>());

	class_<u16_list_intervals>("u16_list_intervals")
		.def("add", u16_add1)
		.def("add", u16_add2)
		.def("aggregate", &u16_list_intervals::aggregate)
		.def("create_index_cache", &u16_list_intervals::create_index_cache)
		.def("size", &u16_list_intervals::size)
		.def("reserve", &u16_list_intervals::reserve)
		.def("clear", &u16_list_intervals::clear)
		.def("at", &u16_list_intervals::at)
		.def("random_sets", &u16_list_random_sets)
		.def("dump_to_file", &u16_list_intervals::dump_to_file)
		.def("read_from_file", &u16_list_intervals::read_from_file)
		.def("__iter__", iterator<u16_list_intervals>());

	class_<u64_list_intervals>("u64_list_intervals")
		.def("add", u64_add1)
		.def("add", u64_add2)
		.def("aggregate", &u64_list_intervals::aggregate)
		.def("create_index_cache", &u64_list_intervals::create_index_cache)
		.def("size", &u64_list_intervals::size)
		.def("reserve", &u64_list_intervals::reserve)
		.def("clear", &u64_list_intervals::clear)
		.def("at", &u64_list_intervals::at)
		.def("random_sets", &u64_list_random_sets)
		.def("dump_to_file", &u64_list_intervals::dump_to_file)
		.def("read_from_file", &u64_list_intervals::read_from_file)
		.def("__iter__", iterator<u64_list_intervals>());

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
		.def("aggregate_properties", &ip_list_intervals_with_properties_python_aggregate_properties)
		.def("aggregate_properties_no_rem", &ip_list_intervals_with_properties_python_aggregate_properties_no_rem)
		.def("property_of", &ip_list_intervals_with_properties_python_property_of_wrapper)
		.def("property_of", &ip_list_intervals_with_properties_python_property_of_wrapper_str)
		.def("create_index_cache", &ip_list_intervals_with_properties_python::create_index_cache)
		.def("size", &ip_list_intervals_with_properties_python::size)
		.def("reserve", &ip_list_intervals_with_properties_python::reserve)
		.def("clear", &ip_list_intervals_with_properties_python::clear)
		.def("at", &ip_list_intervals_with_properties_python::at)
		.def("random_sets", &ip_list_random_sets)
		.def("random_sets_with_properties", &ip_list_with_properties_python_random_sets_with_properties)
		.def("contains", contains1)
		.def("contains", contains2)
		.def("dump_to_file", &ip_list_intervals_with_properties_python::dump_to_file)
		.def("read_from_file", &ip_list_intervals_with_properties_python::read_from_file)
		.def("__iter__", iterator<ip_list_intervals_with_properties_python>());

	class_<u16_set_read_only>("u16_set_read_only")
		.def("at", &u16_set_read_only::at)
		.def("size", &u16_set_read_only::size)
		.def("__iter__", iterator<u16_set_read_only>());

	class_<u32_set_read_only>("u32_set_read_only")
		.def("at", &u32_set_read_only::at)
		.def("size", &u32_set_read_only::size)
		.def("__iter__", iterator<u32_set_read_only>());

	class_<u32_set_properties_read_only_python>("u32_set_properties_read_only_python")
		.def("at", &u32_set_properties_read_only_python::at)
		.def("property_at", &u32_set_properties_read_only_python_property_at_wrapper)
		.def("size", &u32_set_properties_read_only_python::size);

	class_<u64_set_read_only>("u64_set_read_only")
		.def("at", &u64_set_read_only::at)
		.def("size", &u64_set_read_only::size)
		.def("__iter__", iterator<u64_set_read_only>());

	def("ipv4toi", python_ipv4toi1);
	def("ipv4toi", python_ipv4toi2);
}
