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

#include <boost/random.hpp>
#include <boost/python.hpp>

#include <cstdint>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <leeloo/ip_list_intervals.h>
#include <leeloo/ips_parser.h>
#include <leeloo/list_intervals.h>
#include <leeloo/random.h>

using namespace boost::python;

boost::random::mt19937 g_mt_rand;

void (leeloo::ip_list_intervals::*ip_add1)(leeloo::ip_list_intervals::base_type const, leeloo::ip_list_intervals::base_type const) = &leeloo::ip_list_intervals::add;
void (leeloo::ip_list_intervals::*ip_add2)(leeloo::ip_list_intervals::base_type const)                                             = &leeloo::ip_list_intervals::add;
bool (leeloo::ip_list_intervals::*ip_add3)(const char*)                                                                            = &leeloo::ip_list_intervals::add;

void (leeloo::ip_list_intervals::*ip_remove1)(leeloo::ip_list_intervals::base_type const, leeloo::ip_list_intervals::base_type const) = &leeloo::ip_list_intervals::remove;
void (leeloo::ip_list_intervals::*ip_remove2)(leeloo::ip_list_intervals::base_type const)                                             = &leeloo::ip_list_intervals::remove;
bool (leeloo::ip_list_intervals::*ip_remove3)(const char*)                                                                            = &leeloo::ip_list_intervals::remove;

bool (leeloo::ip_list_intervals::*contains1)(uint32_t const) const = &leeloo::ip_list_intervals::contains;
bool (leeloo::ip_list_intervals::*contains2)(const char*)    const = &leeloo::ip_list_intervals::contains;

class set_read_only
{
public:
	typedef uint32_t const* iterator;

public:
	set_read_only():
		_buf(nullptr),
		_size(0)
	{ }

	set_read_only(uint32_t const* buf, size_t const size):
		_buf(buf),
		_size(size)
	{ }

public:
	inline uint32_t at(size_t idx) const
	{
		assert(idx < _size);
		return _buf[idx];
	}

	inline size_t size() const { return _size; }

	uint32_t const* begin() const { return _buf; }
	uint32_t const* end() const   { return _buf+_size; }

private:
	uint32_t const* _buf;
	size_t _size;
};

static void ip_list_random_sets(leeloo::ip_list_intervals const& l, size_t const size_div, object& f_set)
{
	l.random_sets(size_div,
	              [&f_set](uint32_t const* buf, size_t const size) { f_set(set_read_only(buf, size)); },
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

BOOST_PYTHON_MODULE(pyleeloo)
{
	init_rand_gen();

	class_<leeloo::ip_interval>("ip_interval")
		.def("assign", &leeloo::ip_interval::assign)
		.def("lower", &leeloo::ip_interval::lower)
		.def("upper", &leeloo::ip_interval::upper)
		.def("set_lower", &leeloo::ip_interval::set_lower)
		.def("set_upper", &leeloo::ip_interval::set_upper);

	class_<leeloo::ip_list_intervals>("ip_list_intervals")
		.def("add", ip_add1)
		.def("add", ip_add2)
		.def("add", ip_add3)
		.def("remove", ip_remove1)
		.def("remove", ip_remove2)
		.def("remove", ip_remove3)
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

	class_<set_read_only>("set_read_only")
		.def("at", &set_read_only::at)
		.def("size", &set_read_only::size)
		.def("__iter__", iterator<set_read_only>());

	def("ipv4toi", python_ipv4toi1);
	def("ipv4toi", python_ipv4toi2);
}
