#ifndef LEELOO_UPRNG_BASE_H
#define LEELOO_UPRNG_BASE_H

#include <leeloo/random.h>

namespace leeloo {

template <class UPRNG, class Integer, class SeedType>
class uprng_base
{
	typedef UPRNG uprng_type;
	typedef Integer integer_type;
	typedef SeedType seed_type;

	//static_assert(std::is_pod<seed_type>::value, "seed_type must be a POD");

public:
	uprng_base()
	{
		static_cast<uprng_type*>(this)->init_base();
	}

public:
	void init(seed_type const& seed)
	{
		static_cast<uprng_type*>(this)->init_base();
		static_cast<uprng_type*>(this)->init_seed(seed);
	}
	
	template <class Engine>
	void init(integer_type const max, Engine& eng)
	{
		init(seed_type::random(max, eng));
	}

	void init(integer_type const max)
	{
		std::random_device rd;
		init(max, rd);
	}
};


}

#endif
