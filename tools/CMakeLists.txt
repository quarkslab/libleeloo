include_directories(${TBB_INCLUDE_DIRS})

add_executable(leeloo-ip-ranges-rand ip_ranges_rand.cpp)
target_link_libraries(leeloo-ip-ranges-rand leeloo)

add_executable(leeloo-aggregate aggregate.cpp)
target_link_libraries(leeloo-aggregate leeloo)

install(TARGETS leeloo-ip-ranges-rand leeloo-aggregate RUNTIME DESTINATION bin)
