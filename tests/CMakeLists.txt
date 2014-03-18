include_directories(${TBB_INCLUDE_DIRS})

add_executable(test_ips_parser ips_parser.cpp)
target_link_libraries(test_ips_parser leeloo)

add_executable(ip_list_intervals ip_list_intervals.cpp)
target_link_libraries(ip_list_intervals leeloo)

add_definitions(-fopenmp)
add_executable(uni uni.cpp)
target_link_libraries(uni leeloo gomp)

add_executable(uprng uprng.cpp)
target_link_libraries(uprng leeloo gomp)

add_executable(random_sets random_sets.cpp)
target_link_libraries(random_sets leeloo)

add_executable(list_intervals list_intervals.cpp)
target_link_libraries(list_intervals leeloo)

add_executable(list_intervals_properties list_intervals_properties.cpp)
target_link_libraries(list_intervals_properties leeloo)

add_executable(list_intervals_at_perf list_intervals_at_perf.cpp)
target_link_libraries(list_intervals_at_perf leeloo)

add_executable(read_file read_file.cpp)
target_link_libraries(read_file leeloo)

add_executable(dump_file dump_file.cpp)
target_link_libraries(dump_file leeloo)

#add_executable(test_vli test_vli.cpp)
#target_link_libraries(test_vli leeloo vli)
