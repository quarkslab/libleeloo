#include <leeloo/list_intervals.h>
#include <leeloo/interval.h>
#include <leeloo/random.h>
#include <leeloo/uni.h>

int main()
{
	leeloo::list_intervals<leeloo::interval<uint64_t, uint32_t>> l64;
	l64.add(0xFF00000000000000ULL, 0xFF00000000000010ULL);
	l64.create_index_cache(1);
	uint32_t size_sets = 0;
	std::random_device rd;
	l64.random_sets<leeloo::uni>(4,
		[&size_sets](uint64_t const* /*ints*/, const uint32_t size)
		{
			size_sets += size;
		},
		rd);

	if (size_sets != l64.size()) {
		std::cerr << "error: size_sets != real size" << std::endl;
		return 1;
	}

	return 0;
}
