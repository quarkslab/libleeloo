#ifndef LEELOO_INTEGER_CAST_H
#define LEELOO_INTEGER_CAST_H

#include <exception>
#include <type_traits>
#include <limits>

#ifdef LEELOO_MP_SUPPORT
#include <boost/multiprecision/cpp_int.hpp>
#endif

namespace leeloo {

class integer_overflow: public std::exception
{
public:
	virtual const char* what() const throw() override { return "Integer overflow on cast operation!"; }
};

template <class To, class From>
inline To integer_cast(From const& from)
{
	static_assert(std::is_integral<From>::value, "From must be an integer class");
	static_assert(std::is_integral<To>::value,   "To must be an integer class");

	return static_cast<To>(from);
}

template <class To, class From>
inline typename std::enable_if<sizeof(To) >= sizeof(From), To>::type strict_integer_cast(From const& from)
{
	return integer_cast<To>(from);
}

template <class To, class From>
inline typename std::enable_if<sizeof(To) < sizeof(From), To>::type strict_integer_cast(From const& from)
{
	if (from > From(std::numeric_limits<To>::max())) {
		throw integer_overflow();
	}
	if (std::numeric_limits<From>::is_signed && std::numeric_limits<To>::is_signed &&
		(from < From(std::numeric_limits<To>::min()))) {
		throw integer_overflow();
	}
	if (std::numeric_limits<From>::is_signed && !std::numeric_limits<To>::is_signed &&
		(from < 0)) {
		throw integer_overflow();
	}
	return integer_cast<To>(from);
}

#ifdef LEELOO_MP_SUPPORT
template <size_t N> using sint_mp = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<N, N, boost::multiprecision::signed_magnitude,   boost::multiprecision::unchecked, void>>;
template <size_t N> using uint_mp = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<N, N, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;

template <size_t N, bool is_signed> using int_mp = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<N, N, std::conditional<is_signed, typename boost::multiprecision::signed_magnitude, boost::multiprecision::unsigned_magnitude>::type, boost::multiprecision::unchecked, void>>;

template <class To, size_t N, bool is_signed>
inline To integer_cast(int_mp<N, is_signed> const& v)
{
	static_assert(std::is_integral<To>::value, "To must be an integer class");
	return integer_cast<To>(v[0]);
}

template <class To, size_t N, bool is_signed>
inline To strict_integer_cast(int_mp<N, is_signed> const& v)
{
	static_assert(std::is_integral<To>::value, "To must be an integer class");
	for (size_t i = 1; i < N/8; i++) {
		if (v[i] != 0) {
			throw integer_overflow();
		}
	}
	return strict_integer_cast<To>(v[0]);
}
#endif

}

#endif
