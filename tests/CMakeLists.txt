include_directories(${TBB_INCLUDE_DIRS})

set(LINK_LIBRARIES leeloo ${TBB_LIBRARIES} ${Boost_LIBRARIES})

add_executable(test_ips_parser ips_parser.cpp)
target_link_libraries(test_ips_parser ${LINK_LIBRARIES})
add_test(ips_parser test_ips_parser)

add_executable(ip_list_intervals ip_list_intervals.cpp)
target_link_libraries(ip_list_intervals ${LINK_LIBRARIES})
add_test(ip_list_intervals ip_list_intervals)

add_definitions(-fopenmp)
add_executable(uni uni.cpp)
target_link_libraries(uni leeloo gomp)
add_test(uni uni)

add_executable(uprng uprng.cpp)
target_link_libraries(uprng leeloo gomp)
add_test(uprng uprng)

add_executable(random_sets random_sets.cpp)
target_link_libraries(random_sets ${LINK_LIBRARIES})
add_test(random_sets random_sets)

add_executable(list_intervals list_intervals.cpp)
target_link_libraries(list_intervals ${LINK_LIBRARIES})
add_test(list_intervals list_intervals)

add_executable(list_intervals_properties list_intervals_properties.cpp)
target_link_libraries(list_intervals_properties ${LINK_LIBRARIES})
add_test(list_intervals_properties list_intervals_properties)

add_executable(list_intervals_at_perf list_intervals_at_perf.cpp)
target_link_libraries(list_intervals_at_perf ${LINK_LIBRARIES})

add_executable(dump_file dump_file.cpp)
target_link_libraries(dump_file ${LINK_LIBRARIES})
add_test(dump_file dump_file)

add_executable(bit_field bit_field.cpp)
target_link_libraries(bit_field ${LINK_LIBRARIES})
add_test(bit_field bit_field)

add_executable(aggregate_max_prefix aggregate_max_prefix.cpp)
target_link_libraries(aggregate_max_prefix ${LINK_LIBRARIES})
add_test(aggregate_max_prefix aggregate_max_prefix)

add_executable(port_list_intervals port_list_intervals.cpp)
target_link_libraries(port_list_intervals ${LINK_LIBRARIES})
add_test(port_list_intervals port_list_intervals)

add_executable(random_state random_state.cpp)
target_link_libraries(random_state ${LINK_LIBRARIES})
add_test(random_state random_state)

add_executable(integer_cast integer_cast.cpp)
target_link_libraries(integer_cast ${LINK_LIBRARIES})
add_test(integer_cast integer_cast)

#add_executable(test_vli vli.cpp)
#target_link_libraries(test_vli leeloo vli)
