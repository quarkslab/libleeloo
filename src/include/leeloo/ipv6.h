#ifndef LEELOO_IPV6_H
#define LEELOO_IPV6_H

#include <leeloo/config.h>
#include <leeloo/interval.h>
#include <leeloo/integer_mp.h>

namespace leeloo {

#ifdef LEELOO_MP_SUPPORT
typedef uint_mp<128> ipv6_int;
static const ipv6_int ipv6_max_int = std::numeric_limits<ipv6_int>::max();
typedef interval<ipv6_int> ipv6_interval;
#endif

}

#endif
