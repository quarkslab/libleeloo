#ifndef LEELOO_LIST_INTERVALS_PROPERTIES_H
#define LEELOO_LIST_INTERVALS_PROPERTIES_H

#include <leeloo/list_intervals.h>
#include <iostream>

namespace leeloo {

namespace __impl {

template <class Interval, class Property, class SizeType>
class PropertiesHorizontal
{
	typedef Interval interval_type;
	typedef Property property_type;
	typedef SizeType size_type;

	struct Member
	{
		Member(interval_type const& interval, property_type const& property):
			_interval(interval),
			_property(property)
		{ }

		Member(interval_type&& interval, property_type&& property):
			_interval(interval),
			_property(property)
		{ }

		Member(Member const& member):
			_interval(member._interval),
			_property(member._property)
		{ }

		Member(Member&& member):
			_interval(member._interval),
			_property(member._property)
		{ }

		Member& operator=(Member const& o)
		{
			if (&o != this) {
				_interval = o._interval;
				_property = o._property;
			}
			return *this;
		}

		Member& operator=(Member&& o)
		{
			if (&o != this) {
				_interval = std::move(o._interval);
				_property = std::move(o._property);
			}
			return *this;
		}

		interval_type _interval;
		property_type _property;
	};

public:
	inline void add(interval_type const& interval, property_type const& property)
	{
		_properties.emplace_back(Member(interval, property));
	}

	inline void add(interval_type&& interval, property_type&& property)
	{
		_properties.emplace_back(Member(interval, property));
	}

	void sort()
	{
		std::sort(_properties.begin(), _properties.end(),
				[](Member const& a, Member const& b)
				{
					return a._interval.lower() < b._interval.lower();
				});
	}

	inline interval_type const& interval_at(const size_t idx) const
	{
		assert(idx < _properties.size());
		return _properties[idx]._interval;
	}

	inline property_type const& property_at(const size_t idx) const
	{ 
		assert(idx < _properties.size());
		return _properties[idx]._property;
	}

	inline interval_type& interval_at(const size_t idx)
	{
		assert(idx < _properties.size());
		return _properties[idx]._interval;
	}

	inline property_type& property_at(const size_t idx)
	{ 
		assert(idx < _properties.size());
		return _properties[idx]._property;
	}

	inline size_type size() const { return _properties.size(); }

private:
	std::vector<Member> _properties;
};

}

class PropertiesHorizontal
{
public:
	template <class Interval, class Property, class SizeType>
	struct bind
	{
		typedef typename __impl::PropertiesHorizontal<Interval, Property, SizeType> result;
	};
};

template <class Interval, class Property, class SizeType = uint32_t, class PropertiesStorage = PropertiesHorizontal>
class list_intervals_properties: public list_intervals<Interval, SizeType>
{
public:
	typedef Interval interval_type;
	typedef SizeType size_type;
	typedef Property property_type;

	typedef list_intervals<interval_type, size_type> list_intervals_type;
	typedef typename PropertiesStorage::template bind<interval_type, property_type, size_type>::result properties_storage_type;
	typedef typename list_intervals_type::base_type base_type;

public:
	inline void add_property(interval_type const& i, property_type const& p)
	{
		properties().add(i, p);
	}


	inline void add_property(interval_type&& i, property_type&& p)
	{
		properties().add(i, p);
	}

	template <class FMerger>
	void aggregate_properties(FMerger const& fadd, FMerger const& fremove)
	{
		if (properties().size() <= 1) {
			return;
		}

		typedef std::map<base_type, property_type const*> properties_pop_storage

		properties().sort();
		properties_storage_type ret;
		interval_type cur_it = properties().interval_at(0);
		property_type cur_property = properties().property_at(0);
		properties_pop_storage properties_pop;
		properties_pop.insert(std::make_pair(cur_it.upper(), &properties().property_at(0));

		for (size_t i = 1; i < properties().size(); i++) {
			interval_type const& it = properties().interval_at(i);
			property_type const& prop = properties().property_at(i);
			if (it.lower() < cur_it.upper()) {
				interval_type iret(cur_it.lower(), it.lower());
				ret.add(std::move(iret), cur_property);
				
				fadd(cur_property, properties().property_at(i));
				properties_pop.emplace(it.upper(), &prop);
			}
			else {
				ret.add(cur_it, cur_property);
				while (properties_pop.size() > 0) {
					properties_pop_storage::iterator it_first = properties_pop.begin();
					if (it_first->first > it.lower()) {
						break;
					}
					fremove(cur_property, *it_first->second);
					properties_pop_storage::iterator it_next = it_first; it_next++;
					if (it_next != properties_pop.end()) {
						ret.add(interval_type(it_first->first, it_next->end), cur_property);
					}
					properties_pop.erase(it_first);
				}

				if (properties_pop.size() > 0) {
					fadd(cur_property, prop);
					properties_pop.emplace(it.upper(), &prop);

					interval_type iret(it.lower(), properties_pop().begin()->first);
					ret.add(std::move(iret), cur_property);
				}
				else {
					ret.add(it, prop);
				}
			}
		}

		_properties = std::move(ret);

	}

	property_type const* property_of(base_type const& v) const
	{
		// TODO: somehow factorize this code with list_intervals::contains
		size_type a(0);
		size_type b = properties().size(); 

		while ((b-a) > 4) {
			const size_type mid = (b+a)/2;
			interval_type const& it = properties().interval_at(mid);
			if (it.contains(v)) {
				return &properties().property_at(mid);
			}
			if (v < it.lower()) {
				b = mid;
			}
			else {
				a = mid;
			}
		}

		for (size_type i = a; i < b; i++) {
			if (properties().interval_at(i).contains(v)) {
				return &properties().property_at(i);
			}
		}

		return nullptr;
	}

private:
	inline properties_storage_type& properties() { return _properties; }
	inline properties_storage_type const& properties() const { return _properties; }

private:
	properties_storage_type _properties;
};

}

#endif
