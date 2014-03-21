#ifndef LEELOO_LIST_INTERVALS_PROPERTIES_H
#define LEELOO_LIST_INTERVALS_PROPERTIES_H

#include <leeloo/list_intervals.h>
#include <leeloo/bit_field.h>
#include <leeloo/sort_permute.h>

#include <list>

namespace leeloo {

namespace __impl {

template <class Interval, class Property, class SizeType>
class LEELOO_LOCAL PropertiesHorizontal
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
            Member(member._interval, member._property)
		{ }

		Member(Member&& member):
			_interval(std::move(member._interval)),
			_property(std::move(member._property))
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

	typedef std::vector<Member> list_members;

public:
	inline void add(interval_type const& interval, property_type const& property)
	{
		_properties.emplace_back(Member(interval, property));
	}

	inline void add(interval_type&& interval, property_type&& property)
	{
		_properties.emplace_back(Member(interval, property));
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

	void clear_storage()
	{
        // trick to force deallocation of the underlying vector
		_properties.~list_members();
		new (&_properties) list_members();
	}

private:
	list_members _properties;
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
class list_intervals_properties
{
public:
	typedef Interval interval_type;
	typedef SizeType size_type;
	typedef Property property_type;
	typedef typename interval_type::base_type base_type;

	typedef typename PropertiesStorage::template bind<interval_type, property_type, size_type>::result properties_storage_type;

private:
	class LEELOO_LOCAL properties_ir
	{
	public:
		struct elt
		{
			inline property_type const& property(properties_ir const& ir) const
			{
				return ir.property_at(prop_idx);
			}

			inline bool operator<(elt const& o) const { return x < o.x; }

			base_type x;
			size_type prop_idx;
		};

	private:
		typedef std::vector<elt> list_elts;
		typedef std::vector<property_type> list_properties;

	public:
		void add(interval_type const& it, property_type const& prop)
		{
			size_type prop_idx = _properties.size();
			_properties.push_back(prop);
			add(it, prop_idx);
		}

		void add(interval_type const& it, property_type&& prop)
		{
			size_type prop_idx = _properties.size();
			_properties.emplace_back(prop);
			add(it, prop_idx);
		}

		void reserve(size_type const n)
		{
			_ir.reserve(n*2);
			_actions.reserve(n*2);
			_properties.reserve(n);
		}

		void sort()
		{
			std::sort(make_sort_permute_iter(_ir.begin(), _actions.begin()),
				   	  make_sort_permute_iter(_ir.end(), _actions.begin()+size_elts()),
					  sort_permute_iter_compare<typename list_elts::iterator, typename bit_field::iterator>());
		}

		void clear_storage()
		{
			// AG: it seems that the only way to free memory of an std::vector
			// of glibc is to call its destructor.
			_ir.~list_elts();
			new (&_ir) list_elts();

			_properties.~list_properties();
			new (&_properties) list_properties();

			_actions.clear_storage();
		}

		inline property_type const& property_at(size_type i) const { return _properties[i]; }
		inline elt const& elt_at(size_type i) const { return _ir[i]; }
		inline bool action_at(size_type i) const { return _actions.get_bit_fast(i); }

		size_type size_elts() const { return _ir.size(); }

	private:
		void add(interval_type const& it, size_type const prop_idx)
		{
			elt e = {it.lower(), prop_idx};
			_actions.set_bit(_ir.size());
			_ir.push_back(e);

			e.x = it.upper();
			_actions.clear_bit(_ir.size());
			_ir.push_back(e);
		}

	private:
		list_elts _ir;
		list_properties _properties;
		// Bit is at 1 for push, 0 for pop
		bit_field _actions;
	};

	struct no_property_duplicate
	{
		template <class T>
		inline T const& operator()(T const& t) const { return t; }
	};

public:
	inline void add_property(interval_type const& i, property_type const& p)
	{
		ir().add(i, p);
	}


	inline void add_property(interval_type const& i, property_type&& p)
	{
		ir().add(i, std::move(p));
	}

	inline void add_property(base_type const a, base_type const b, property_type const& p)
	{
		add_property(interval_type(a, b), p);
	}

	inline void add_property(base_type const a, base_type const b, property_type&& p)
	{
		add_property(interval_type(a, b), std::move(p));
	}

	template <class FAdd, class FRemove>
	void aggregate_properties(FAdd const& fadd, FRemove const& fremove)
	{
		aggregate_properties(fadd, fremove, no_property_duplicate());
	}

	template <class FAdd, class FRemove, class FDuplicate>
	void aggregate_properties(FAdd const& fadd, FRemove const& fremove, FDuplicate const& fdup)
	{
		if (ir().size_elts() == 0) {
			properties().clear_storage();
			return;
		}
		
		assert(ir().size_elts() % 2 == 0);

		ir().sort();
		typename properties_ir::elt const& first_elt = ir().elt_at(0);
		base_type prev_value = first_elt.x;
		property_type cur_property = fdup(first_elt.property(ir()));

		for (size_type i = 1; i < ir().size_elts(); i++) {
			typename properties_ir::elt const& elt = ir().elt_at(i);
			const bool action = ir().action_at(i);

			_properties.add(interval_type(prev_value, elt.x), fdup(cur_property));
			if (action) {
				// Add the elt property to the current property
				// TODO: std::move the property ?
				fadd(cur_property, elt.property(ir()));
			}
			else {
				// Remove the elt property to the current property
				// TODO: std::move the property ?
				fremove(cur_property, elt.property(ir()));
			}
			prev_value = elt.x;
		}

		// Free memory taken by the IR
		ir().clear_storage();
	}

	template <class FAdd>
	void aggregate_properties_no_rem(FAdd const& fadd)
	{
		aggregate_properties_no_rem(fadd, no_property_duplicate());
	}

	template <class FAdd, class FDuplicate>
	void aggregate_properties_no_rem(FAdd const& fadd, FDuplicate const& fdup)
	{
		if (ir().size_elts() == 0) {
			properties().clear_storage();
			return;
		}
		
		assert(ir().size_elts() % 2 == 0);

		ir().sort();
		typename properties_ir::elt const& first_elt = ir().elt_at(0);
		base_type prev_value = first_elt.x;
		std::list<property_type const*> cur_properties;
		cur_properties.push_back(&first_elt.property(ir()));
		property_type cur_property = fdup(first_elt.property(ir()));

		for (size_type i = 1; i < ir().size_elts(); i++) {
			typename properties_ir::elt const& elt = ir().elt_at(i);
			const bool action = ir().action_at(i);

			_properties.add(interval_type(prev_value, elt.x), fdup(cur_property));
			property_type const& pelt = elt.property(ir());
			if (action) {
				// Merge the elt property
				if (cur_properties.size() == 0) {
					cur_property = fdup(pelt);
				}
				else {
					fadd(cur_property, pelt);
				}
				// And add it to the list of current properties
				cur_properties.push_back(&pelt);
			}
			else {
				// Remove the elt property from the list of current properties
				typename std::list<property_type const*>::iterator it_prop = std::find(cur_properties.begin(), cur_properties.end(), &pelt);
				assert(it_prop != cur_properties.end());
				cur_properties.erase(it_prop);
				// And remerge everything
				cur_property = std::move(merge_properties(cur_properties, fadd, fdup));
			}
			prev_value = elt.x;
		}

		// Free memory taken by the IR
		ir().clear_storage();
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
	template <class FAdd, class FDuplicate>
	LEELOO_LOCAL property_type merge_properties(std::list<property_type const*> const& props, FAdd const& fadd, FDuplicate const& fdup)
	{
		property_type ret;
		if (props.size() == 0) {
			return ret;
		}
		typename std::list<property_type const*>::const_iterator it = props.begin();
		ret = fdup(*(*it));
		it++;
		for (; it != props.end(); it++) {
			fadd(ret, *(*it));
		}
		return std::move(ret);
	}

private:
	LEELOO_LOCAL inline properties_storage_type& properties() { return _properties; }
	LEELOO_LOCAL inline properties_storage_type const& properties() const { return _properties; }

	LEELOO_LOCAL inline properties_ir& ir() { return _properties_ir; }
	LEELOO_LOCAL inline properties_ir const& ir() const { return _properties_ir; }

private:
	properties_storage_type _properties;
	properties_ir _properties_ir;
};

}

#endif
