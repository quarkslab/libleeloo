#ifndef LEELOO_INTEGER_CAST_H
#define LEELOO_INTEGER_CAST_H

#include <exception>
#include <type_traits>
#include <limits>

#ifdef LEELOO_VLI_SUPPORT
#include <vli/integer.hpp>
#endif

#include <leeloo/integer_traits.h>

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

#ifdef LEELOO_VLI_SUPPORT
template <class To, size_t N>
inline To integer_cast(vli::integer<N> const& vli)
{
	static_assert(std::is_integral<To>::value, "To must be an integer class");
	return integer_cast<To>(vli[0]);
}

template <class To, size_t N>
inline To strict_integer_cast(vli::integer<N> const& vli)
{
	static_assert(std::is_integral<To>::value, "To must be an integer class");
	for (size_t i = 1; i < vli::integer<N>::numwords; i++) {
		if (vli[i] != 0) {
			throw integer_overflow();
		}
	}
	return strict_integer_cast<To>(vli[0]);
}
#endif

template <class T>
typename integer_above<T>::type integer_cast_above(T const& v)
{
	typedef typename integer_above<T>::type above_type;
	return integer_cast<above_type>(v);
}

}

#endif
