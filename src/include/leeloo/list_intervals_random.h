#ifndef LEELOO_LIST_INTERVALS_RANDOM_H
#define LEELOO_LIST_INTERVALS_RANDOM_H

#include <random>

#include <leeloo/config.h>
#include <leeloo/list_intervals.h>
#include <leeloo/interval.h>

#ifdef LEELOO_BOOST_SERIALIZE
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
#endif

namespace leeloo {

template <class ListIntervals, template <class T_, bool atomic_> class UPRNG, bool atomic = false>
class list_intervals_random
{
	typedef ListIntervals list_intervals_type;
	typedef typename ListIntervals::base_type base_type;
	typedef typename ListIntervals::difference_type difference_type;
	typedef UPRNG<difference_type, atomic> uprng_type;

public:
	typedef typename uprng_type::seed_type seed_type;

public:
	void init(seed_type const seed_, difference_type const step = 0)
	{
		_seed = seed_;
		_uprng.init(seed_);
		_cur_step = step;
	}

	template <class RandEngine>
	void init(list_intervals_type const& li, RandEngine& rand_engine)
	{
		_seed = seed_type::random(li.size(), rand_engine);
		init(_seed);
	}

	void init(list_intervals_type const& li)
	{
		_seed = seed_type::random(li.size());
		init(_seed);
	}

	base_type operator()(list_intervals_type const& li)
	{
		difference_type const n = _uprng.get_step(_cur_step);
		_cur_step++;
		return li.at_cached(n);
	}

	base_type get_step(list_intervals_type const& li, difference_type const& step)
	{
		return li.at_cached(_uprng.get_step(step));
	}

	bool end() const { return _cur_step == size_todo(); }
	difference_type size_original() const { return _uprng.max(); }
	difference_type size_todo() const { return _uprng.max(); }
	difference_type cur_step() const { return _cur_step; }
	seed_type const& seed() const { return _seed; }

#ifdef LEELOO_BOOST_SERIALIZE
	template <class Archive>
	void save_state(Archive& ar)
	{
		ar << boost::serialization::make_nvp("seed", _seed);
		ar << boost::serialization::make_nvp("cur_step", _cur_step);
	}

	template <class Archive>
	void restore_state(Archive& ar)
	{
		ar >> boost::serialization::make_nvp("seed", _seed);
		ar >> boost::serialization::make_nvp("cur_step", _cur_step);
		_uprng.init(_seed);
	}
#endif

private:
	uprng_type _uprng;
	difference_type _cur_step;
	seed_type _seed;
};

template <class ListIntervals, template <class T_, bool atomic_> class UPRNG, bool atomic = false>
class list_intervals_random_promise
{
	typedef ListIntervals list_intervals_type;
	typedef typename ListIntervals::base_type base_type;
public:
	// TODO: should force the unsigned version of it
	typedef typename ListIntervals::difference_type difference_type;

private:
	typedef UPRNG<difference_type, atomic> uprng_type;
	typedef list_intervals<interval<base_type>, base_type> steps_list_intervals;

public:
	typedef typename uprng_type::seed_type seed_type;

public:
	void init(seed_type const& seed, difference_type step_start, difference_type step_end)
	{
		_seed = seed;
		_uprng.init(seed);
		step_end = std::min(step_end, _uprng.max());
		step_start = std::min(step_start, _uprng.max());
		if (step_start > step_end) {
			std::swap(step_start, step_end);
		}

		_steps_todo.clear();
		_steps_todo.add(step_start, step_end);

		_done_steps.clear();
		_done_steps.add(0, step_start);
		_done_steps.add(step_end, _uprng.max());
		_done_steps.aggregate();

		_it_steps = _steps_todo.value_begin();
	}

	void init(seed_type const& seed)
	{
		init(seed, 0, seed.max());
	}

	template <class RandEngine>
	void init(list_intervals_type const& li, RandEngine& rand_engine)
	{
		init(seed_type::random(li.size(), rand_engine));
	}

	void init(list_intervals_type const& li)
	{
		init(seed_type::random(li.size()));
	}

	base_type operator()(list_intervals_type const& li)
	{
		base_type const ret = get_current(li);
		next();
		return ret;
	}

	inline base_type get_current(list_intervals_type const& li) const
	{
		return li.at_cached(_uprng.get_step(*_it_steps));
	}

	inline difference_type get_current_step() const
	{
		return *_it_steps;
	}

	void step_done(difference_type const step)
	{
		assert(end() || (step <= *_it_steps));
		_done_steps.add(interval<difference_type>(step, step+1));
		if (const_cast<steps_list_intervals const&>(_done_steps).intervals().size() >= 100) {
			_done_steps.aggregate();
		}
	}

	bool end() const { return _it_steps == _steps_todo.value_end(); }
	difference_type size_original() const { return _uprng.max(); }
	difference_type size_todo() const { return _steps_todo.size(); }
	difference_type size_done() const { return _done_steps.size(); }

	steps_list_intervals const& done_steps() const { return _done_steps; }

	void aggregate_done_steps() { _done_steps.aggregate(); }

	void set_done_steps(steps_list_intervals const& done_steps)
	{
		_done_steps = done_steps;
		_steps_todo.clear();
		_steps_todo.add(0, _uprng.max());
		_done_steps.aggregate();
		for (interval<difference_type> const& i: _done_steps) {
			_steps_todo.remove(i);
		}
		_steps_todo.aggregate();
		_it_steps = _steps_todo.value_begin();
	}

	inline void next()
	{
		assert(!end());
		++_it_steps;
	}

#ifdef LEELOO_BOOST_SERIALIZE
	template <class Archive>
	void save_state(Archive& ar)
	{
		ar << boost::serialization::make_nvp("seed", _seed);

		_done_steps.aggregate();
		ar << boost::serialization::make_nvp("done_steps", _done_steps);
	}

	template <class Archive>
	void restore_state(Archive& ar)
	{
		ar >> boost::serialization::make_nvp("seed", _seed);
		ar >> boost::serialization::make_nvp("done_steps", _done_steps);

		_uprng.init(_seed);
		set_done_steps(_done_steps);
	}
#endif

private:
	uprng_type _uprng;
	steps_list_intervals _steps_todo;
	steps_list_intervals _done_steps;
	typename steps_list_intervals::value_iterator _it_steps;
	seed_type _seed;
};

}

#ifdef leeloo_EXPORTS

#ifdef LEELOO_INCLUDE_U32
extern template class LEELOO_API leeloo::list_intervals_random<leeloo::list_intervals<leeloo::interval<uint32_t>>, leeloo::uni, false>;
#endif

#endif

#endif
