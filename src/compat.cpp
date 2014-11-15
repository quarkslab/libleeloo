#include <leeloo/compat.h>
#include <cstdlib>

void* leeloo::aligned_malloc(size_t alignment, size_t size)
{
#ifdef WIN32
	return _aligned_malloc(alignment, size);
#else
	void* ret;
	int ret = posix_memalign(&ret, alignment, size);
	if (ret != 0) {
		return nullptr;
	}
	return ret;
#endif
}

void leeloo::aligned_free(void* ptr)
{
#ifdef WIN32
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}