include_directories(${TBB_INCLUDE_DIRS})

add_executable(ip_ranges_rand ip_ranges_rand.cpp)
target_link_libraries(ip_ranges_rand leeloo)

add_executable(aggregate aggregate.cpp)
target_link_libraries(aggregate leeloo)
