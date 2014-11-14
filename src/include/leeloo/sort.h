#ifndef LEELOO_SORT_H
#define LEELOO_SORT_H

#include <leeloo/config.h>

#ifdef HAS_TBB
#include <tbb/parallel_sort.h>
#endif

namespace leeloo {

#ifdef HAS_TBB

template <class... Args>
static inline void parallel_or_serial_sort(Args && ... args)
{
	tbb::parallel_sort(std::forward<Args>(args)...);
}

#else

template <class... Args>
static inline void parallel_or_serial_sort(Args && ... args)
{
	std::sort(std::forward<Args>(args)...);
}

#endif

} // leeloo

#endif
