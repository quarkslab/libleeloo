#ifndef LEELOO_LIST_INTERVALS_RANDOM_H
#define LEELOO_LIST_INTERVALS_RANDOM_H

#include <boost/random/random_device.hpp>

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
	typedef typename ListIntervals::size_type size_type;
	typedef UPRNG<size_type, atomic> uprng_type;

public:
	typedef uint32_t seed_type;

public:
	template <class RandEngine>
	void init(list_intervals_type const& li, RandEngine&& rand_engine, seed_type const seed_, size_type const step = 0)
	{
		_seed = seed_;
		rand_engine.seed(_seed);
		_uprng.init(li.size(), rand_engine);
		_cur_step = step;
	}

	template <class RandEngine>
	void init(list_intervals_type const& li, RandEngine&& rand_engine)
	{
		init(li, rand_engine, boost::random::random_device()());
	}

	base_type operator()(list_intervals_type const& li)
	{
		size_type const n = _uprng.get_step(_cur_step);
		_cur_step++;
		return li.at_cached(n);
	}

	bool end() const { return _cur_step == size_todo(); }
	size_type size_original() const { return _uprng.max(); }
	size_type size_todo() const { return _uprng.max(); }
	size_type cur_step() const { return _cur_step; }
	seed_type seed() const { return _seed; }

#ifdef LEELOO_BOOST_SERIALIZE
	template <class Archive>
	void save_state(Archive& ar)
	{
		ar << boost::serialization::make_nvp("seed", _seed);
		ar << boost::serialization::make_nvp("cur_step", _cur_step);
	}

	template <class Archive, class RandEngine>
	void restore_state(Archive& ar, list_intervals_type const& li, RandEngine&& rand_engine)
	{
		ar >> boost::serialization::make_nvp("seed", _seed);
		ar >> boost::serialization::make_nvp("cur_step", _cur_step);
		rand_engine.seed(_seed);
		_uprng.init(li.size(), rand_engine);
	}
#endif

private:
	uprng_type _uprng;
	size_type _cur_step;
	seed_type _seed;
};

template <class ListIntervals, template <class T_, bool atomic_> class UPRNG, bool atomic = false>
class list_intervals_random_promise
{
	typedef ListIntervals list_intervals_type;
	typedef typename ListIntervals::base_type base_type;
public:
	// TODO: should force the unsigned version of it
	typedef typename ListIntervals::size_type size_type;

private:
	typedef UPRNG<size_type, atomic> uprng_type;
	typedef list_intervals<interval<base_type>, base_type> steps_list_intervals;

public:
	typedef uint32_t seed_type;

public:
	template <class RandEngine>
	void init(list_intervals_type const& li, RandEngine&& rand_engine, seed_type const seed, size_type step_start, size_type step_end)
	{
		_seed = seed;
		rand_engine.seed(_seed);
		_uprng.init(li.size(), rand_engine);
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

	template <class RandEngine>
	void init(list_intervals_type const& li, RandEngine&& rand_engine, seed_type const seed)
	{
		init(li, rand_engine, seed, 0, li.size());
	}

	template <class RandEngine>
	void init(list_intervals_type const& li, RandEngine&& rand_engine)
	{
		_seed = boost::random::random_device()();
		init(li, rand_engine, _seed);
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

	inline size_type get_current_step() const
	{
		return *_it_steps;
	}

	void step_done(size_type const step)
	{
		assert(end() || (step <= *_it_steps));
		_done_steps.add(interval<size_type>(step, step+1));
		if (const_cast<steps_list_intervals const&>(_done_steps).intervals().size() >= 100) {
			_done_steps.aggregate();
		}
	}

	bool end() const { return _it_steps == _steps_todo.value_end(); }
	size_type size_original() const { return _uprng.max(); }
	size_type size_todo() const { return _steps_todo.size(); }
	size_type size_done() const { return _done_steps.size(); }

	steps_list_intervals const& done_steps() const { return _done_steps; }

	void aggregate_done_steps() { _done_steps.aggregate(); }

	void set_done_steps(steps_list_intervals const& done_steps)
	{
		_done_steps = done_steps;
		_steps_todo.clear();
		_steps_todo.add(0, _uprng.max());
		_done_steps.aggregate();
		for (interval<size_type> const& i: _done_steps) {
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

	template <class Archive, class RandEngine>
	void restore_state(Archive& ar, list_intervals_type const& li, RandEngine&& rand_engine)
	{
		ar >> boost::serialization::make_nvp("seed", _seed);
		ar >> boost::serialization::make_nvp("done_steps", _done_steps);

		rand_engine.seed(_seed);
		_uprng.init(li.size(), rand_engine);
		set_done_steps(_done_steps);

		rand_engine.seed(_seed);
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
