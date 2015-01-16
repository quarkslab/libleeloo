#ifndef LEELOO_INTEGER_TRAITS_H
#define LEELOO_INTEGER_TRAITS_H

#include <leeloo/config.h>
#include <leeloo/integer_mp.h>

#include <boost/integer.hpp> 

#include <type_traits>
#include <limits>

namespace leeloo {

template <class T>
class integer_bits
{
	static_assert(std::is_integral<T>::value, "T must be an integer");

	template <size_t N_>
	struct static_N
	{
		static constexpr size_t N = N_;
	};

public:
	static constexpr bool is_signed = std::numeric_limits<T>::is_signed;
	static constexpr size_t N = std::conditional<is_signed,
					 static_N<std::numeric_limits<T>::digits+1>,
					 static_N<std::numeric_limits<T>::digits>>::type::N;
};

template <class T>
struct is_integer_mp: public std::false_type
{ };

#ifdef LEELOO_MP_SUPPORT
template <unsigned N_>
struct integer_bits<boost_uint_mp<N_>>
{
	static constexpr size_t N = N_;
	static constexpr bool is_signed = true;
};

template <unsigned N_>
struct integer_bits<boost_sint_mp<N_>>
{
	static constexpr size_t N = N_;
	static constexpr bool is_signed = false;
};

template <unsigned N>
struct is_integer_mp<boost_uint_mp<N>>: public std::true_type
{ };

template <unsigned N>
struct is_integer_mp<boost_sint_mp<N>>: public std::true_type
{ };
#endif

namespace __impl {

template <class T>
class integer_above
{
	static_assert(std::numeric_limits<T>::is_integer, "T must be an integer");
	typedef integer_bits<T> integer_bits_type;

public:
	typedef typename std::conditional<
				integer_bits_type::is_signed,
				typename boost::int_t<integer_bits_type::N*2>::exact,
				typename boost::uint_t<integer_bits_type::N*2>::exact>::type type;
};

template <>
struct integer_above<int64_t>
{
#ifdef LEELOO_MP_SUPPORT
	typedef sint_mp<128> type;
#else
	// LEELOO_MP_SUPPORt must be activated to support integers above 64-bits!
	typedef void type;
#endif
};

template <>
struct integer_above<uint64_t>
{
#ifdef LEELOO_MP_SUPPORT
	typedef uint_mp<128> type;
#else
	// LEELOO_MP_SUPPORt must be activated to support integers above 64-bits!
	typedef void type;
#endif
};

#ifdef LEELOO_MP_SUPPORT
// In case int_mp<128, *> is promoted to a special intrinsic type!
template <>
struct integer_above<sint_mp<128>>
{
	typedef sint_mp<256> type;
};

template <>
struct integer_above<uint_mp<128>>
{
	typedef uint_mp<256> type;
};
#endif

#ifdef LEELOO_MP_SUPPORT
template <unsigned N>
struct integer_above<boost_uint_mp<N>>
{
	typedef boost_uint_mp<N*2> type;
};

template <unsigned N>
struct integer_above<boost_sint_mp<N>>
{
	typedef boost_sint_mp<N*2> type;
};
#endif

} // __impl

template <class T>
struct integer_above: public __impl::integer_above<T>
{ };

#ifdef LEELOO_MP_SUPPORT
template <class T>
struct is_integral_or_mp: public std::is_integral<T>
{ };

template <unsigned N>
struct is_integral_or_mp<boost_uint_mp<N>>: public std::true_type
{ };

template <unsigned N>
struct is_integral_or_mp<boost_sint_mp<N>>: public std::true_type
{ };
#else
template <class T>
struct is_integral_or_mp: public std::is_integral<T>
{ };
#endif
} // leeloo

#endif
