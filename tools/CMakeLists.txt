include_directories(${TBB_INCLUDE_DIRS})

set(LINK_LIBRARIES leeloo ${TBB_LIBRARIES} ${Boost_LIBRARIES})

add_executable(leeloo-ip-ranges-rand ip_ranges_rand.cpp)
target_link_libraries(leeloo-ip-ranges-rand ${LINK_LIBRARIES})

add_executable(leeloo-aggregate aggregate.cpp)
target_link_libraries(leeloo-aggregate ${LINK_LIBRARIES})

install(TARGETS leeloo-ip-ranges-rand leeloo-aggregate RUNTIME DESTINATION bin)
