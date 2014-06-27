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

#ifndef LEELOO_INTERVAL_LIST_H
#define LEELOO_INTERVAL_LIST_H

#include <tbb/parallel_sort.h>

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <exception>
#include <iterator>
#include <iostream>

#include <leeloo/bench.h>
#include <leeloo/config.h>
#include <leeloo/exports.h>
#include <leeloo/uni.h>
#include <leeloo/utility.h>
#include <leeloo/integer_cast.h>

#ifdef LEELOO_BOOST_SERIALIZE
#include <boost/serialization/vector.hpp>
#include <boost/serialization/split_member.hpp>
#endif

namespace leeloo {

class LEELOO_API file_exception: public std::exception
{
public:
	file_exception():
		_errno(errno),
		_what(strerror(errno))
	{ }

public:
	const char* what() const throw() override { return _what.c_str(); }
	int get_errno() const { return _errno; }

private:
	int _errno;
	std::string _what;
};

class LEELOO_API file_format_exception: public std::exception
{
public:
	file_format_exception(const char* msg):
		_msg(msg)
	{ }

public:
	const char* what() const throw() override { return _msg; }

private:
	const char* _msg;
};

template <class Interval, class SizeType = uint32_t>
class list_intervals 
{
public:
	typedef Interval interval_type;
	typedef SizeType size_type;

	typedef typename interval_type::base_type base_type;
	typedef list_intervals<Interval, SizeType> this_type;

	static constexpr unsigned int interger_bits = sizeof(base_type)*CHAR_BIT;

private:
	struct cmp_f
	{
		inline bool operator()(interval_type const& i1, interval_type const& i2) const
		{
			return i1.lower() < i2.lower();
		}
	};

public:
	typedef std::vector<interval_type> container_type;
	typedef typename container_type::const_iterator iterator;

private:
	struct tag_vi_end
	{ };

public:
	// Const value iterator
	class value_iterator: std::iterator<std::forward_iterator_tag, base_type>
	{
		typedef std::iterator<std::forward_iterator_tag, base_type> iterator_base_type;
	public:
		value_iterator()
		{
			// undefined values
		}

		value_iterator(container_type const& v):
			_iter(v.begin()),
			_offset(0)
		{
		}

		value_iterator(container_type const& v, tag_vi_end):
			_iter(v.end()),
			_offset(0)
		{
		}

	public:
		value_iterator& operator++()
		{
			if (_iter->lower() + _offset != _iter->upper() - 1) {
				++_offset;
			}
			else {
				++_iter;
				_offset = 0;
			}
			return *this;
		}

		value_iterator operator++(int)
		{
			value_iterator ret = *this;
			++*this;
			return ret;
		}

		typename iterator_base_type::value_type operator*() const
		{
			return _iter->lower() + _offset;
		}

		bool operator!=(value_iterator const& other) const
		{
			return _iter != other._iter or _offset != other._offset;
		}

		bool operator==(value_iterator const& other) const
		{
			return _iter == other._iter and _offset == other._offset;
		}

	private:
		iterator _iter;
		size_type _offset;
	};

public:
	list_intervals():
		_cache_entry_size(0)
	{ }

public:
	inline void add(base_type const a, base_type const b)
	{
		intervals().emplace_back(a, b);
	}

	inline void add(interval_type const& i)
	{
		intervals().push_back(i);
	}

	inline void remove(base_type const a, base_type const b)
	{
		removed_intervals().emplace_back(a, b);
	}

	inline void remove(interval_type const& i)
	{
		removed_intervals().push_back(i);
	}

	// The two following functions suppose that the other intervals had been
	// aggregated.
	inline void add(list_intervals const& o)
	{
		// AG: I'm afraid that insert isn't optimized here... That's one
		// limitation of std::*.
		intervals().reserve(intervals().size() + o.intervals().size());
		intervals().insert(intervals().end(), o.intervals().begin(), o.intervals().end());
	}

	inline void remove(list_intervals const& o)
	{
		// AG: same remark as above
		removed_intervals().reserve(removed_intervals().size() + o.intervals().size());
		removed_intervals().insert(removed_intervals().end(), o.intervals().begin(), o.intervals().end());
	}

	template <bool exclude = false>
	inline void insert(typename std::enable_if<exclude == true, interval_type const&>::type i)
	{
		remove(i);
	}

	template <bool exclude = false>
	inline void insert(typename std::enable_if<exclude == false, interval_type const&>::type i)
	{
		add(i);
	}

	template <bool exclude = false>
	inline void insert(typename std::enable_if<exclude == true, base_type const>::type a, base_type const b)
	{
		remove(a, b);
	}

	template <bool exclude = false>
	inline void insert(typename std::enable_if<exclude == false, base_type const>::type a, base_type const b)
	{
		add(a, b);
	}

	void aggregate()
	{
		if (removed_intervals().size() == 0) {
			aggregate_container(intervals());
			return;
		}

		if (intervals().size() == 0) {
			return;
		}

		aggregate_container(removed_intervals());

		tbb::parallel_sort(intervals().begin(), intervals().end(), cmp_f());

		container_type ret;
		interval_type cur_merge = intervals().front();
		typename container_type::const_iterator it_removed = removed_intervals().begin();

		typename container_type::const_iterator it = intervals().begin()+1;

		for (; it != intervals().end(); it++) {
			interval_type const& cur_int = *it;
			if (utility::overlap(cur_merge, cur_int)) {
				cur_merge = utility::hull(cur_merge, cur_int);
			}
			else {
				// split 'cur_merge' according to removed_intervals
				split_merged_interval_removed(cur_merge, ret, it_removed);
				cur_merge = cur_int;
			}
		}

		// Now, merge the removed intervals, and do the final merge
		split_merged_interval_removed(cur_merge, ret, it_removed);

		// Clear the removed intervals
		removed_intervals().clear();

		intervals() = std::move(ret);
	}

	void aggregate_max_prefix(unsigned int const min_prefix)
	{
		if (min_prefix == 0) {
			return;
		}

		aggregate();

		const base_type mask = prefix2mask(min_prefix);
		const base_type inv_mask = ~mask;
		const base_type max_size = mask+1;

		base_type prev_a = std::numeric_limits<base_type>::min();
		base_type prev_b = std::numeric_limits<base_type>::max();
		// Do not use iterator as we will append intervals, and thus modify the end!
		const size_type org_size = intervals().size();
		for (size_type i = 0; i < org_size; i++) {
			interval_type const& it = intervals()[i];
			if (it.width() < max_size) {
				base_type const a = it.lower() & inv_mask;
				base_type const b = (it.upper() | mask) + 1;
				if (a != prev_a || b != prev_b) {
					add(a, b);
					prev_a = a;
					prev_b = b;
				}
			}
		}

		aggregate();
	}

	static inline base_type prefix2mask(const unsigned int prefix)
	{
		if (prefix == 0) {
			return ~(base_type(0));
		}
		return (base_type(1)<<(interger_bits-prefix))-1;
	}

	base_type size() const
	{
		base_type ret(0);
		for (interval_type const& i: intervals()) {
			ret += i.width();
		}
		return ret;
	}

	template <template <class T_, bool atomic_> class UPRNG, class Fset, class RandEngine>
	void random_sets(size_type size_div, Fset const& fset, RandEngine const& rand_eng) const
	{
		if (size_div <= 0) {
			size_div = 1;
		}
		const base_type size_all = size();
		UPRNG<base_type, false> uprng;
		uprng.init(size_all, rand_eng);

		base_type* interval_buf;
		posix_memalign((void**) &interval_buf, 16, sizeof(base_type)*size_div);
		if (interval_buf == nullptr) {
			return;
		}

		const size_type size_all_full = strict_integer_cast<size_type>(size_all/base_type(size_div))*size_div;
		for (size_type i = 0; i < size_all_full; i += size_div) {
			for (size_t j = 0; j < size_div; j++) {
				interval_buf[j] = at_cached(uprng());
			}
			fset(interval_buf, size_div);
		}

		const size_type rem = integer_cast<size_type>(size_all-base_type(size_all_full));
		if (rem > 0) {
			for (size_type i = size_all_full; i < size_all; i++) {
				interval_buf[i-size_all_full] = at_cached(uprng());
			}
			fset(interval_buf, rem);
		}

		free(interval_buf);
	}

	template <class Fset, class RandEngine>
	inline void random_sets(size_type size_div, Fset const& fset, RandEngine const& rand_eng) const
	{
		random_sets<uni>(size_div, fset, rand_eng);
	}

	inline void reserve(size_type n) { intervals().reserve(n); }
	inline void clear() { intervals().clear(); removed_intervals().clear(); }

	inline container_type const& intervals() const { return _intervals; }

	inline base_type at(size_type const r) const
	{
		assert(r < size());
		return get_rth_value(r, 0, intervals().size());
	}

	base_type at_cached(base_type const r) const
	{
		assert(r < size() && _cache_entry_size > 0);
		ssize_t cur;
		const size_t interval_idx = get_cached_interval_idx(r, cur);
		if (cur == 0) {
			return intervals()[interval_idx].lower();
		}
		return get_rth_value(cur, interval_idx, intervals().size());
	}

	// cache_entry_size defines the number of intervals that represent a cache entry
	void create_index_cache(size_t const cache_entry_size)
	{
		assert(cache_entry_size > 0);
		_cache_entry_size = cache_entry_size;
		const size_t intervals_count = intervals().size();
		size_type cur_size(0);
		_index_cache.clear();
		_index_cache.reserve((intervals_count+cache_entry_size-1)/cache_entry_size);
		for (size_t i = 0; i < intervals_count; i += cache_entry_size) {
			const size_t j_end = std::min(intervals_count, i+cache_entry_size);
			for (size_t j = i; j < j_end; j++) {
				cur_size += intervals()[j].width();
			}
			_index_cache.push_back(cur_size);
		}
	}

	bool operator==(this_type const& o) const
	{
		if (_intervals.size() != o._intervals.size()) {
			return false;
		}

		for (size_t i = 0; i < _intervals.size(); i++) {
			if ((_intervals[i].lower() != o._intervals[i].lower()) ||
				(_intervals[i].upper() != o._intervals[i].upper())) {
				return false;
			}
		}

		return true;
	}

	inline bool operator!=(this_type const& o) const
	{
		return !operator==(o);
	}

	bool contains(base_type const v) const
	{
		// Suppose that intervals have been aggregated! (and are thus sorted)
		size_type a(0);
		size_type b = intervals().size(); 

		while ((b-a) > 4) {
			const size_type mid = (b+a)/2;
			interval_type const& it = intervals()[mid];
			if (it.contains(v)) {
				return true;
			}
			if (v < it.lower()) {
				b = mid;
			}
			else {
				a = mid;
			}
		}

		for (size_type i = a; i < b; i++) {
			if (intervals()[i].contains(v)) {
				return true;
			}
		}

		return false;
	}

	/*
	std::vector<this_type> divide_by(size_type const n) const
	{
		std::vector<this_type> ret;
		const base_type whole_size = size();
		const size_type values_per_obj = strict_integer_cast<size_type>((whole_size+n-1)/n);

		ret.resize(n);
		std::vector<this_type>::iterator cur_obj;
		size_type cur_count = 0;

		for (interval_type const& it: intervals()) {
			cur_count += it.size();
			if (cur_count < values_per_obj) {
				cur_obj->add(it);
			}
			else {
				const size_t above = cur_count - values_per_obj;
				cur_obj->add(interval_type(it.lower(), it.lower()+above));

				++cur_obj;
				if (cur_obj == ret.end()) {
					return ret;
				}
				interval_type rem(interval_type(it.lower()+above, it.upper());
				cur_obj.add(rem);
				cur_count = rem.size();
			}
		}
		return ret;

	}*/

public:
	void read_stream(std::istream& os)
	{
		uint32_t nintervals;
		os >> nintervals;
		clear();
		intervals().resize(nintervals);
		os.read((char*) &intervals()[0], nintervals*sizeof(interval_type));
	}

	void dump_stream(std::ostream& os)
	{
		uint32_t nintervals = strict_integer_cast<uint32_t>(intervals().size());
		os << nintervals;
		os.write((const char*) &intervals()[0], intervals().size()*sizeof(interval_type));
	}

	void dump_to_fd(int fd)
	{
		if (fd == -1) {
			throw file_exception();
		}

		size_t size_write = intervals().size()*sizeof(interval_type);
		ssize_t w = write(fd, &intervals()[0], size_write);
		if ((w < 0) ||
		    (((size_t)w) != size_write)) {
			throw file_exception();
		}
	}

	void read_from_fd(int fd)
	{
		off_t size = lseek(fd, 0, SEEK_END);
		if (size == -1) {
			throw file_exception();
		}
		if (lseek(fd, 0, SEEK_SET) == -1) {
			throw file_exception();
		}

		if (size % sizeof(interval_type) != 0) {
			throw file_format_exception("invalid size");
		}

		size_t n = size/sizeof(interval_type);
		clear();
		intervals().resize(n);
		ssize_t r = read(fd, &intervals()[0], size);
		if (r < 0) {
			throw file_exception();
		}
		if (r != size) {
			throw file_exception();
		}

		for (interval_type const& i: intervals()) {
			if (i.lower() >= i.upper()) {
				throw file_format_exception("invalid interval");
			}
		}
	}

	void dump_to_file(const char* file)
	{
		int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
		if (fd == -1) {
			throw file_exception();
		}

		dump_to_fd(fd);

		close(fd);
	}

	void read_from_file(const char* file)
	{
		int fd = open(file, O_RDONLY);
		if (fd == -1) {
			throw file_exception();
		}

		read_from_fd(fd);

		close(fd);
	}

#ifdef LEELOO_BOOST_SERIALIZE
	template<class Archive>
	void save(Archive& ar, unsigned int const /*version*/) const
	{
		ar & boost::serialization::make_nvp("intervals", intervals());
	}

	template<class Archive>
	void load(Archive& ar, unsigned int const /*version*/)
	{
		clear();
		ar & boost::serialization::make_nvp("intervals", intervals());
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif

private:
	static void aggregate_container(container_type& ints)
	{
		if (ints.size() <= 1) {
			return;
		}
		
		tbb::parallel_sort(ints.begin(), ints.end(), cmp_f());

		container_type ret;
		interval_type cur_merge = ints.front();

		typename container_type::const_iterator it = ints.begin()+1;

		// TODO: there is plenty of room for memory optimisations,
		// vectorisation and //isation ;)
		for (; it != ints.end(); it++) {
			interval_type const& cur_int = *it;
			if (utility::overlap(cur_merge, cur_int)) {
				cur_merge = utility::hull(cur_merge, cur_int);
			}
			else {
				ret.push_back(cur_merge);
				cur_merge = cur_int;
			}
		}

		ret.push_back(cur_merge);

		ints = std::move(ret);
	}

	void split_merged_interval_removed(interval_type const& cur_merge_, container_type& ret, typename container_type::const_iterator& it_removed)
	{
		bool notend;
		while (((notend = it_removed != removed_intervals().end())) &&
				(cur_merge_.lower() > it_removed->upper())) {
			it_removed++;
		}

		interval_type cur_merge = cur_merge_;
		if (notend) {
			typename container_type::const_iterator it_removed_cur = it_removed;
			do {
				const base_type rem_lower = it_removed_cur->lower();
				const base_type rem_upper = it_removed_cur->upper();
				if ((cur_merge.lower() >= rem_lower) &&
					(cur_merge.upper() <= rem_upper)) {
					cur_merge = interval_type::empty();
				}
				else
				if ((cur_merge.lower() >= rem_lower) &&
					(cur_merge.upper() >= rem_upper) &&
					(cur_merge.lower() <= rem_upper)) {
					cur_merge.set_lower(rem_upper);
				}
				else
				if ((cur_merge.lower() <= rem_lower) &&
					(cur_merge.upper() <= rem_upper) &&
					(cur_merge.upper() >= rem_lower)) {
					cur_merge.set_upper(rem_lower);
				}
				else
				if ((cur_merge.lower() <= rem_lower) &&
					(cur_merge.upper() >= rem_upper)) {
					ret.emplace_back(cur_merge.lower(), rem_lower);
					cur_merge.set_lower(rem_upper);
				}

				it_removed_cur++;
			}
			while ((it_removed_cur != removed_intervals().end()) && (it_removed_cur->lower() < cur_merge.upper()));
		}

		if (cur_merge.width() > 0) {
			ret.push_back(cur_merge);
		}
	}

	size_type get_rth_value(base_type const r, size_t const interval_start, size_t const interval_end) const
	{
		// [interval_start,interval_end[
		base_type cur = r;
		for (size_t i = interval_start; i < interval_end; i++) {
			const interval_type it = intervals()[i];
			const base_type it_width = it.width();
			if (cur < it_width) {
				// This is it!
				return it.upper() - (it_width - cur);
			}
			else {
				cur -= it_width;
			}
		}
		return -1;
	}

	size_t get_cached_interval_idx(size_type const r, ssize_t& rem) const
	{
		// Dichotomy!
		size_t a = 0;
		size_t b = _index_cache.size();

		// Tradeoff
		while ((b-a) > 4) {
			const size_t mid = (b+a)/2;
			const size_type vmid = _index_cache[mid];
			if (vmid == r) {
				// You get lucky
				rem = 0;
				return (mid+1)*_cache_entry_size;
			}
			if (vmid < r) {
				a = mid;
			}
			else {
				b = mid;
			}
		}

		// Finish this!
		size_type v;
		while ((v = _index_cache[a]) < r) {
			a++;
		}

		if (a > 0) {
			rem = (ssize_t)r-(ssize_t)_index_cache[a-1];
		}
		else {
			rem = r;
		}

		return a*_cache_entry_size;
	}

public:
	iterator begin() const { return intervals().begin(); }
	iterator end() const { return intervals().end(); }

	value_iterator value_begin() const { return value_iterator(_intervals); }
	value_iterator value_end()   const { return value_iterator(_intervals, tag_vi_end()); }

private:
	LEELOO_LOCAL inline container_type& intervals() { return _intervals; }
	LEELOO_LOCAL inline container_type& removed_intervals() { return _excluded_intervals; }
	LEELOO_LOCAL inline container_type const& removed_intervals() const { return _excluded_intervals; }

private:
	container_type _intervals;
	container_type _excluded_intervals;
	std::vector<size_type> _index_cache;
	size_t _cache_entry_size;
};

}

// Common exported instanciations (outside of any namespace)
//

#ifdef LEELOO_INCLUDE_U8
LEELOO_TEMPLATE_EXPIMP template class LEELOO_API leeloo::list_intervals<leeloo::interval<uint8_t>>;
#endif

#ifdef LEELOO_INCLUDE_U16
LEELOO_TEMPLATE_EXPIMP template class LEELOO_API leeloo::list_intervals<leeloo::interval<uint16_t>>;
#endif

#ifdef LEELOO_INCLUDE_U32
LEELOO_TEMPLATE_EXPIMP template class LEELOO_API leeloo::list_intervals<leeloo::interval<uint32_t>>;
#endif

#endif
