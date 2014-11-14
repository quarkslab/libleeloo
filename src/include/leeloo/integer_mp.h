#ifndef LEELOO_INTEGER_MP_H
#define LEELOO_INTEGER_MP_H

#include <leeloo/config.h>

#ifdef LEELOO_MP_SUPPORT
#include <boost/multiprecision/cpp_int.hpp>
#endif

namespace leeloo {

#ifdef LEELOO_MP_SUPPORT 

template <unsigned N, boost::multiprecision::cpp_integer_type int_type> using boost_int_mp = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<N, N, int_type,  boost::multiprecision::unchecked, void>>;
template <unsigned N> using boost_sint_mp = boost_int_mp<N, boost::multiprecision::signed_magnitude>;
template <unsigned N> using boost_uint_mp = boost_int_mp<N, boost::multiprecision::unsigned_magnitude>;

namespace __impl {

template <unsigned N, boost::multiprecision::cpp_integer_type int_type>
struct mp_chooser
{
	typedef boost_int_mp<N, int_type> type;
};

// TODO: use __int128_t when possible
#if 0
template <>
struct mp_chooser<128, boost::multiprecision::signed_magnitude>
{
	typedef __int128_t type;
};

template <>
struct mp_chooser<128, boost::multiprecision::unsigned_magnitude>
{
	typedef __uint128_t type;
};
#endif


} // __impl

template <unsigned N, boost::multiprecision::cpp_integer_type int_type> using int_mp = typename __impl::mp_chooser<N, int_type>::type;

template <unsigned N> using sint_mp = int_mp<N, boost::multiprecision::signed_magnitude>;
template <unsigned N> using uint_mp = int_mp<N, boost::multiprecision::unsigned_magnitude>;

#endif

} // leeloo

#endif
