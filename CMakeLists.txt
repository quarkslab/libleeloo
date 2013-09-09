cmake_minimum_required(VERSION 2.8)
project(leeloo)

include(CMakeCompilers.txt)
include(CMakeRequiredLibraries.txt)

include_directories(src/include)

add_subdirectory(src)
add_subdirectory(bindings)
add_subdirectory(tests)
add_subdirectory(tools)
