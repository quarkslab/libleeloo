#include <leeloo/integer_traits.h>
#include <iostream>

#ifdef LEELOO_MP_SUPPORT
typedef leeloo::uint_mp<128> uint128_t;
typedef leeloo::sint_mp<128> int128_t;
#endif

int main()
{
	static_assert(std::is_same<leeloo::integer_above<int32_t>::type, int64_t>::value,    "above(int32_t) != int64_t");
	static_assert(std::is_same<leeloo::integer_above<uint32_t>::type, uint64_t>::value,  "above(uint32_t) != uint64_t");
#ifdef LEELOO_MP_SUPPORT
	static_assert(std::is_same<leeloo::integer_above<int64_t>::type, int128_t>::value,   "above(int64_t) != int128_t");
	static_assert(std::is_same<leeloo::integer_above<uint64_t>::type, uint128_t>::value, "above(uint64_t) != uint128_t");
	static_assert(std::is_same<leeloo::integer_above<int128_t>::type, leeloo::sint_mp<256>>::value,  "above(int128_t) != int256_t");
	static_assert(std::is_same<leeloo::integer_above<uint128_t>::type, leeloo::uint_mp<256>>::value, "above(uint128_t) != uint256_t");
#endif

	return 0;
}
