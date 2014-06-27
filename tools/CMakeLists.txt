include_directories(${TBB_INCLUDE_DIRS})

add_executable(ip_ranges_rand ip_ranges_rand.cpp)
target_link_libraries(ip_ranges_rand leeloo)

add_executable(aggregate-leeloo aggregate.cpp)
target_link_libraries(aggregate-leeloo leeloo)

install(TARGETS ip_ranges_rand aggregate-leeloo RUNTIME DESTINATION bin)
