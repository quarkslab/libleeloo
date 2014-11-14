#include <boost/multiprecision/cpp_int.hpp>
#include <leeloo/interval.h>
#include <leeloo/list_intervals.h>
#include <leeloo/random.h>
#include <leeloo/uni.h>
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
	test.random_sets<leeloo::uni>(64,
		[&size_sets](uint128_t const* /*ints*/, const uint32_t size)
		{
			size_sets += size;
		},
		rd);

	if (size_sets != test.size()) {
		std::cerr << "invalid returned size for random_sets" << std::endl;
		ret = 1;
	}

	{
		leeloo::uni<uint64_t> uni;
		uni.init(0xFFFFFFFFFFULL);
		for (size_t i = 0; i < 10; i++) {
			std::cout << uni() << std::endl;
		}
	}

	{
		leeloo::uni<uint128_t> uni;
		uni.init(0xFFFFFFFFFFULL);
		for (size_t i = 0; i < 10; i++) {
			std::cout << uni() << std::endl;
		}
	}

	leeloo::list_intervals<leeloo::interval<uint128_t, uint128_t>> test2;
	test2.add(0, 0xFFFF);
	a = 0xFFFFFFFFFFFFFFFFULL;
	a *= 5;
	test2.add(a, a*100000);
	test2.aggregate();
	test2.create_index_cache(1);
	std::cout << test2.size() << std::endl;
	 
	//This compiles but takes a huge amount of time :/ 
#if 0
	uint128_t size_sets2 = 0;
	test2.random_sets<leeloo::uni>(64,
		[&size_sets2](uint128_t const*, uint128_t const& size)
		{
			size_sets2 += size;
		},
		rd);
	if (size_sets2 != test2.size()) {
		std::cerr << "invalid size of two 128-bit intervals" << std::endl;
		ret = 1;
	}
#endif

	return ret;
}
