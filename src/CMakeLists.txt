set(SRC_FILES
	helpers.cpp
	ip_list_intervals.cpp
	ips_parser.cpp
	list_intervals.cpp
)

set(HEADER_DIST_FILES
	include/leeloo/atomic_helpers.h
	include/leeloo/bench.h
	include/leeloo/bits_permutation.h
	include/leeloo/helpers.h
	include/leeloo/interval.h
	include/leeloo/intrinsics.h
	include/leeloo/ip_list_intervals.h
	include/leeloo/ips_parser.h
	include/leeloo/list_intervals.h
	include/leeloo/math_helpers.h
	include/leeloo/prime_helpers.h
	include/leeloo/uni.h
	include/leeloo/uprng.h
	include/leeloo/utility.h
)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${TBB_INCLUDE_DIR})

foreach (INC ${LEELOO_INCLUDE})
	add_definitions(-DLEELOO_INCLUDE_U${INC})
endforeach()

add_library(leeloo SHARED ${SRC_FILES})
target_link_libraries(leeloo ${TBB_LIBRARIES})

install(TARGETS leeloo LIBRARY DESTINATION lib)
install(FILES ${HEADER_DIST_FILES} DESTINATION include/leeloo)
