#include <boost/multiprecision/cpp_int.hpp>
#include <leeloo/interval.h>
#include <leeloo/list_intervals.h>
#include <leeloo/random.h>
#include <leeloo/uprng.h>
#include <leeloo/integer_cast.h>

#include <random>

//typedef boost::multiprecision::uint128_t uint128_t;
typedef leeloo::uint_mp<128> uint128_t;
typedef leeloo::sint_mp<128> int128_t;

int main()
{
	int ret = 0;

	leeloo::list_intervals<leeloo::interval<uint128_t, uint32_t>> test;
	test.add(0, 0xFFFF);
	uint128_t a = 0xFFFFFFFFFFFFFFFFULL;
	a *= 5;
	test.add(a, a + 124567);
	test.aggregate();
	test.create_index_cache(1);
	if (test.size() != (124567 + 0xFFFF)) {
		std::cerr << "invalid size of two intervals" << std::endl;
		ret = 1;
	}

	uint32_t size_sets = 0;
	std::random_device rd;
	test.random_sets<leeloo::uprng>(64,
		[&size_sets](uint128_t const* /*ints*/, const uint32_t size)
		{
			size_sets += size;
		},
		leeloo::random_engine<uint32_t>(rd));

	if (size_sets != test.size()) {
		std::cerr << "invalid returned size for random_sets" << std::endl;
		ret = 1;
	}


	return ret;
}
