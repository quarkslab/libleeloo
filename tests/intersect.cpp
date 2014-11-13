#include <leeloo/list_intervals.h>
#include <leeloo/interval.h>

#include <iostream>

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

	list_intervals li, li2;

	li.clear();
	li.add(0, 10);
	li2.add(5, 20);
	li.intersect(li2);
	{
		uint32_t ref[] = {
			5, 10
		};
		ret |= compare_intervals(li, ref, 1);
	}

	li.clear();
	li2.clear();
	li.add(10, 200);
	li.add(400, 1000);
	li2.add(50, 600);
	li.intersect(li2);
	{
		uint32_t ref[] = {
			50, 200,
			400, 600
		};
		ret |= compare_intervals(li, ref, 2);
	}

	return ret;
}
