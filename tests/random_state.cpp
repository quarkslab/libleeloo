#include <leeloo/interval.h>
#include <leeloo/list_intervals.h>
#include <leeloo/list_intervals_random.h>
#include <leeloo/random.h>

#ifdef LEELOO_BOOST_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#endif

#include <iostream>
#include <sstream>

typedef leeloo::interval<uint32_t> interval;
typedef leeloo::list_intervals<interval, uint32_t> list_intervals;
typedef leeloo::list_intervals_random<list_intervals, leeloo::uni> list_intervals_random;
typedef leeloo::list_intervals_random_promise<list_intervals, leeloo::uni> list_intervals_random_promise;

int main()
{
	int ret = 0;
	list_intervals list;
	list.add(interval(0, 100));
	list.add(interval(200, 300));
	list.add(interval(500, 600));
	list.add(interval(800, 900));
	list.aggregate();
	list.create_index_cache(32);

	list_intervals_random lir;
	boost::random::mt19937 gen(time(NULL));
	lir.init(list, leeloo::random_engine<uint32_t>(gen));
	list_intervals_random::seed_type const seed = lir.seed();
	std::vector<uint32_t> ref;
	const size_t lsize = list.size();
	ref.reserve(lsize);
	for (uint32_t i = 0; !lir.end(); i++) {
		ref.push_back(lir(list));
	}
	if (ref.size() != lsize) {
		std::cerr << "Error on the end condition" << std::endl;
		// stops here as something is really wrong...
		return 1;
	}
	for (int i = 0; i < 10; i++) {
		std::cout << ref[i] << std::endl;
	}

	lir.init(list, leeloo::random_engine<uint32_t>(gen), seed);
	for (uint32_t i = 0; i < lsize; i++) {
		if (ref[i] != lir(list)) {
			std::cout << "bad value at index " << i << std::endl;
			ret = 1;
		}
	}

	lir.init(list, leeloo::random_engine<uint32_t>(gen), seed);
	list_intervals_random lir2;
	lir2.init(list, leeloo::random_engine<uint32_t>(gen), seed, lsize/2);
	for (uint32_t i = 0; i < lsize/2; i++) {
		lir(list);
	}
	for (uint32_t i = lsize/2; i < lsize; i++) {
		uint32_t v = lir(list);
		if (v != lir2(list)) {
			std::cerr << "Error on index " << i << " with custom step" << std::endl;
			ret = 1;
		}
		if (v != ref[i]) {
			std::cerr << "Value differs from the difference for " << i << std::endl;
			ret = 1;
		}
	}

#ifdef LEELOO_BOOST_SERIALIZE
	lir2.init(list, leeloo::random_engine<uint32_t>(gen), seed, lsize/2);

	std::stringstream ss;
	boost::archive::text_oarchive oa(ss);
	lir2.save_state(oa);

	list_intervals_random lir_boost;
	ss.seekg(0, std::stringstream::beg);
	boost::archive::text_iarchive ia(ss);
	lir_boost.restore_state(ia, list, leeloo::random_engine<uint32_t>(gen));

	for (uint32_t i = lsize/2; i < lsize; i++) {
		uint32_t v = lir2(list);
		if (v != lir_boost(list)) {
			std::cerr << "Error on index " << i << " with custom step after serialisatoin" << std::endl;
			ret = 1;
		}
		if (v != ref[i]) {
			std::cerr << "Value differs from the difference for " << i << std::endl;
			ret = 1;
		}
	}
#endif

	list_intervals_random_promise lirp;
	lirp.init(list, leeloo::random_engine<uint32_t>(gen), seed);

	uint32_t j;
	for (j = 0; !lirp.end(); j++) {
		if (j >= lsize) {
			std::cerr << "error: random_promise::end didn't returned true when it should have been at " << j << std::endl;
			// stop everything now...
			return 1;
		}
		if (lirp(list) != ref[j]) {
			std::cerr << "random_promise give different results than random at " << j << std::endl;
			ret = 1;
		}
		lirp.step_done(j);
	}
	if (j != lsize) {
		std::cerr << "error: random_promise::end returned too soon!" << std::endl;
		return 1;
	}

	lirp.aggregate_done_steps();
	if (lirp.done_steps().intervals().size() != 1) {
		std::cerr << "done_steps is more than one interval!" << std::endl;
		// stops here as something is really bad!
		return 1;
	}
	interval const& ds = *lirp.done_steps().begin();
	if (ds.lower() != 0 || ds.upper() != lsize) {
		std::cerr << "done_steps interval isn't [0, " << lsize << "[" << std::endl;
		// stops here as something is really bad!
		return 1;
	}

	lirp.init(list, leeloo::random_engine<uint32_t>(gen), seed);
	for (uint32_t i = 0; i < lsize/2; i++) {
		lirp(list);
		if (i % 7 != 0) {
			lirp.step_done(i);
		}
	}
	lirp.aggregate_done_steps();
	list_intervals done_steps = lirp.done_steps();

	lirp.init(list, leeloo::random_engine<uint32_t>(gen), seed);
	lirp.set_done_steps(done_steps);
	for (uint32_t i = 0; i < lsize/2; i++) {
		if (i % 7 == 0) {
			if (lirp(list) != ref[i]) {
				std::cerr << "random_promies gave wrong results after reinitialisation at " << i << std::endl;
				ret = 1;
			}
		}
	}

	for (uint32_t i = lsize/2; i < lsize; i++) {
		if (lirp(list) != ref[i]) {
			std::cerr << "random_promies gave wrong results at " << i << std::endl;
			ret = 1;
		}
	}

#ifdef LEELOO_BOOST_SERIALIZE
	lirp.init(list, leeloo::random_engine<uint32_t>(gen), seed);
	for (uint32_t i = 0; i < lsize/2; i++) {
		lirp(list);
		if (i % 7 != 0) {
			lirp.step_done(i);
		}
	}
	std::stringstream ss2;
	boost::archive::text_oarchive oa2(ss2);
	lirp.save_state(oa2);

	list_intervals_random_promise lirp_boost;
	ss2.seekg(0, std::stringstream::beg);
	boost::archive::text_iarchive ia2(ss2);
	lirp_boost.restore_state(ia2, list, leeloo::random_engine<uint32_t>(gen));

	for (uint32_t i = 0; i < lsize/2; i++) {
		if (i % 7 == 0) {
			if (lirp_boost(list) != ref[i]) {
				std::cerr << "random_promies gave wrong results after reinitialisation at " << i << std::endl;
				ret = 1;
			}
		}
	}

	for (uint32_t i = lsize/2; i < lsize; i++) {
		if (lirp_boost(list) != ref[i]) {
			std::cerr << "random_promies gave wrong results at " << i << std::endl;
			ret = 1;
		}
	}
#endif

	return ret;
}
