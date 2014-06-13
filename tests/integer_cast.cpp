#include <leeloo/integer_cast.h>
#ifdef LEELOO_VLI_SUPPORT
#include <vli/integer.hpp>
#endif

#include <iostream>

template <class Exception, class Func>
bool check_throw(Func const& f)
{
	bool ret = false;
	try {
		f();
	}
	catch (Exception const&) {
		ret = true;
	}
	return ret;
}

int main()
{
	int ret = 0;
	
	ret = !check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<char>(10000); });
	if (ret == 1) {
		std::cerr << "strict_integer_cast<char>(10000) should throw!" << std::endl;
	}

	ret = check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<char>(5); });
	if (ret == 1) {
		std::cerr << "strict_integer_cast<char>(5) shouldn't throw!" << std::endl;
	}

	ret = check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<char>(-5); });
	if (ret == 1) {
		std::cerr << "strict_integer_cast<char>(-5) shouldn't throw!" << std::endl;
	}

	ret = !check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<char>(-200000); });
	if (ret == 1) {
		std::cerr << "strict_integer_cast<char>(-200000) should throw!" << std::endl;
	}

	ret = check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<size_t>(200000); });
	if (ret == 1) {
		std::cerr << "strict_integer_cast<size_t>(200000) shouldn't throw!" << std::endl;
	}

	ret = check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<ssize_t>(200000); });
	if (ret == 1) {
		std::cerr << "strict_integer_cast<ssize_t>(-200000) shouldn't throw!" << std::endl;
	}

#ifdef LEELOO_VLI_SUPPORT
	ret = check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<int>(vli::integer<128>(10)); });
	if (ret == 1) {
		std::cerr << "strict_integer_cast<int>(vli(10)) shouldn't throw!" << std::endl;
	}

	vli::integer<128> bigint;
	bigint[0] = 0xFFFFFFFFULL;
	bigint[1] = 0xFFFFFFFFULL;
	ret = !check_throw<leeloo::integer_overflow>([&bigint] { leeloo::strict_integer_cast<unsigned int>(bigint); });
	if (ret == 1) {
		std::cerr << "strict_integer_cast<int>(vli(10)) should throw!" << std::endl;
	}
#endif


	{
		auto test = leeloo::integer_cast_above(static_cast<uint32_t>(4));
		static_assert(std::is_same<decltype(test), uint64_t>::value, "integer_cast_above unsigned 32 bits failed");
	}

	{
		auto test = leeloo::integer_cast_above(static_cast<int32_t>(4));
		static_assert(std::is_same<decltype(test), int64_t>::value, "integer_cast_above signed 32 bits failed");
	}

#ifdef LEELOO_VLI_SUPPORT
	{
		auto test = leeloo::integer_cast_above(static_cast<uint64_t>(4));
		static_assert(std::is_same<decltype(test), vli::integer<128>>::value, "integer_cast_above above 64 bits failed");
	}

	{
		auto test = leeloo::integer_cast_above(vli::integer<128>(4));
		static_assert(std::is_same<decltype(test), vli::integer<256>>::value, "integer_cast_above vli 128 bits failed");
	}
#endif

	return 0;
}
