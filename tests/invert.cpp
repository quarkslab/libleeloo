#include <leeloo/list_intervals.h>
#include <leeloo/interval.h>

#include <iostream>

#define N 1000

typedef leeloo::list_intervals<leeloo::interval<uint32_t>> list_intervals;

template <class Interval>
int compare_intervals(Interval const& l, typename Interval::base_type const* const ref, size_t const ninter)
{
	if (l.intervals().size() != ninter) {
		std::cerr << "bad number of intervals" << std::endl;
		return 1;
	}
	int ret = 0;
	size_t i = 0;
	for (auto const& it: l.intervals()) {
		if ((it.lower() != ref[2*i]) ||
			(it.upper() != ref[2*i+1])) {
			std::cerr << "invalid interval at index " << i << std::endl;
			ret = 1;
		}
		i++;
	}
	return ret;
}


int main()
{
	srand(time(NULL));

	int ret = 0;

	list_intervals li;
	li.add(0, 5);

	list_intervals li_i = li.invert();
	{
		uint32_t ref[] = {
			5, 0xFFFFFFFFUL
		};
		ret |= compare_intervals(li_i, ref, 1);
	}

	li.clear();
	li.add(100, 0xFFFFFFFFUL);
	li_i = li.invert();
	{
		uint32_t ref[] = {
			0, 100
		};
		ret |= compare_intervals(li_i, ref, 1);
	}

	li.clear();
	li.add(0, 10);
	li.add(20, 400);
	li.add(1000, 0xFFFFFFFFUL);
	li_i = li.invert();
	{
		uint32_t ref[] = {
			10, 20,
			400, 1000
		};
		ret |= compare_intervals(li_i, ref, 2);
	}

	li.clear();
	li.add(20, 400);
	li.add(1000, 2000);
	li_i = li.invert();
	{
		uint32_t ref[] = {
			0, 20,
			400, 1000,
			2000, 0xFFFFFFFFUL
		};
		ret |= compare_intervals(li_i, ref, 3);
	}

	li.clear();
	li.add(0, 20);
	li.add(20, 400);
	li.add(4000, 8000);
	li.aggregate();
	li_i = li.invert();
	{
		uint32_t ref[] = {
			400, 4000,
			8000, 0xFFFFFFFFUL
		};
		ret |= compare_intervals(li_i, ref, 2);
	}

	li.add(li_i);
	li.aggregate();
	{
		uint32_t ref[] = {
			0, 0xFFFFFFFFUL
		};
		ret |= compare_intervals(li, ref, 1);
	}

	li.clear();
	li.reserve(N);
	for (size_t i = 0; i < N; i++) {
		uint32_t a = rand();
		uint32_t b = rand();
		if (a > b) {
			std::swap(a, b);
		}
		li.add(a, b);
	}
	li.aggregate();
	li_i = li.invert();
	li.add(li_i);
	li.aggregate();
	{
		uint32_t ref[] = {
			0, 0xFFFFFFFFUL
		};
		ret |= compare_intervals(li, ref, 1);
	}

	return ret;
}
