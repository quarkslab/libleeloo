#ifndef LEELOO_COMPAT_H
#define LEELOO_COMPAT_H

#include <leeloo/exports.h>
#include <cstdlib>

#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

namespace leeloo {

LEELOO_API void* aligned_malloc(size_t alignment, size_t size);
LEELOO_API void aligned_free(void* ptr);

}

#endif
