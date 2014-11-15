#include <leeloo/config.h>
#include <leeloo/integer_cast.h>

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
	int tmp;
	
	tmp = !check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<char>(10000); });
	if (tmp == 1) {
		std::cerr << "strict_integer_cast<char>(10000) should throw!" << std::endl;
		ret = 1;
	}

	tmp = check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<char>(5); });
	if (tmp == 1) {
		std::cerr << "strict_integer_cast<char>(5) shouldn't throw!" << std::endl;
		ret = 1;
	}

	tmp = check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<char>(-5); });
	if (tmp == 1) {
		std::cerr << "strict_integer_cast<char>(-5) shouldn't throw!" << std::endl;
		ret = 1;
	}

	tmp = !check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<char>(-200000); });
	if (tmp == 1) {
		std::cerr << "strict_integer_cast<char>(-200000) should throw!" << std::endl;
		ret = 1;
	}

	tmp = check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<size_t>(200000); });
	if (tmp == 1) {
		std::cerr << "strict_integer_cast<size_t>(200000) shouldn't throw!" << std::endl;
		ret = 1;
	}

	tmp = check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<ssize_t>(200000); });
	if (tmp == 1) {
		std::cerr << "strict_integer_cast<ssize_t>(-200000) shouldn't throw!" << std::endl;
		ret = 1;
	}

#ifdef LEELOO_MP_SUPPORT
	typedef leeloo::uint_mp<128> uint128_t;

	tmp = check_throw<leeloo::integer_overflow>([] { leeloo::strict_integer_cast<ssize_t>(uint128_t(200000)); });
	if (tmp == 1) {
		std::cerr << "strict_integer_cast<size_t>(uint128_t(1000)) shouldn't throw!" << std::endl;
		ret = 1;
	}

	uint128_t bigint(0xFFFFFFFFFFFFFFFFULL);
	bigint *= 4;
	tmp = !check_throw<leeloo::integer_overflow>([&bigint] { leeloo::strict_integer_cast<size_t>(bigint); });
	if (tmp == 1) {
		std::cerr << "strict_integer_cast<size_t>(uint128_t(1000)) should throw!" << std::endl;
		ret = 1;
	}
#endif

	return ret;
}
