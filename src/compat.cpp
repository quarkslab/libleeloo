#include <leeloo/compat.h>
#include <cstdlib>

void* leeloo::aligned_malloc(size_t alignment, size_t size)
{
#ifdef WIN32
	return _aligned_malloc(size, alignment);
#else
	void* ptr;
	int ret = posix_memalign(&ptr, alignment, size);
	if (ret != 0) {
		return nullptr;
	}
	return ptr;
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
