#include <leeloo/interval.h>
#include <leeloo/list_intervals.h>
#include <leeloo/uni.h>
#include <leeloo/random.h>

#include <boost/random.hpp>
#include <vli/integer.hpp>

typedef vli::integer<128> u128;
typedef leeloo::interval<u128> u128_interval;
typedef leeloo::list_intervals<u128_interval> u128_list_intervals;

int main()
{
	boost::random::mt19937 mt_rand(time(NULL));
	u128_list_intervals li;
	li.add(u128(10), u128(20));
	li.aggregate();
	li.random_sets<leeloo::uni>(16, [](u128 const* buf, size_t const n) { },
			leeloo::random_engine<uint32_t>(mt_rand));

	return 0;
}
