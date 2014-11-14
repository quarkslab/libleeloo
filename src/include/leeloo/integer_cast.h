#ifndef LEELOO_INTEGER_CAST_H
#define LEELOO_INTEGER_CAST_H

#include <exception>
#include <type_traits>
#include <limits>

#include <leeloo/integer_mp.h>
#include <leeloo/integer_traits.h>

namespace leeloo {

class integer_overflow: public std::exception
{
public:
	virtual const char* what() const throw() override { return "Integer overflow on cast operation!"; }
};


namespace __impl {

template <class To, class From>
struct integer_cast_impl_base
{
	static_assert(is_integral_or_mp<From>::value, "From must be an integer class");
	static_assert(is_integral_or_mp<To>::value,   "To must be an integer class");

	// "natural" cast used when possible (i.e. if From==To)
	inline static To cast(From const from)
	{
		return static_cast<To>(from);
	}
};

template <class To, class From, bool to_bigger_from>
struct integer_cast_impl;

template <class To, class From>
struct integer_cast_impl<To, From, true>: public integer_cast_impl_base<To, From>
{
	typedef integer_cast_impl_base<To, From> base_type;

	inline static To strict_cast(From const from)
	{
		return base_type::cast(from);
	}
};

template <class To, class From>
struct integer_cast_impl<To, From, false>: public integer_cast_impl_base<To, From>
{
	typedef integer_cast_impl_base<To, From> base_type;

	inline static To strict_cast(From const from)
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
		return base_type::cast(from);
	}
};

#ifdef LEELOO_MP_SUPPORT
template <class To, unsigned N, boost::multiprecision::cpp_integer_type int_type>
struct integer_cast_impl<To, boost_int_mp<N, int_type>, false>
{
	typedef int_mp<N, int_type> from_type;
	static_assert(is_integral_or_mp<To>::value, "To must be an integer class");

	inline static To cast(from_type const& v)
	{
		return v.template convert_to<To>();
	}

	inline static To strict_cast(from_type const from)
	{
		if (from > from_type(std::numeric_limits<To>::max())) {
			throw integer_overflow();
		}
		if (std::numeric_limits<from_type>::is_signed && std::numeric_limits<To>::is_signed &&
			(from < from_type(std::numeric_limits<To>::min()))) {
			throw integer_overflow();
		}
		if (std::numeric_limits<from_type>::is_signed && !std::numeric_limits<To>::is_signed &&
			(from < 0)) {
			throw integer_overflow();
		}
		return cast(from);
	}
};
#endif

template <class To, class From>
struct integer_cast: public integer_cast_impl<To, From, sizeof(To) >= sizeof(From)>
{ };

#ifdef LEELOO_MP_SUPPORT
template <unsigned N, boost::multiprecision::cpp_integer_type int_type>
struct integer_cast<boost_int_mp<N, int_type>, boost_int_mp<N, int_type>>
{
	typedef boost_int_mp<N, int_type> integer_type;

	inline static integer_type const& cast(integer_type const& v) { return v; }
	inline static integer_type const& strict_cast(integer_type const& v) { return v; }
};
#endif

} // __impl

template <class To, class From>
inline To integer_cast(From const& from)
{
	return __impl::integer_cast<To, From>::cast(from);
}

template <class To, class From>
inline To strict_integer_cast(From const& from)
{
	return __impl::integer_cast<To, From>::strict_cast(from);
}

} // leeloo

#endif
