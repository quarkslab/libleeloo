cmake_minimum_required(VERSION 2.8)
project(leeloo)

enable_testing()

if (NOT LEELOO_BOOST_SERIALIZE)
	set(LEELOO_BOOST_SERIALIZE 0)
endif()

if (NOT LEELOO_VLI_SUPPORT)
	set(LEELOO_VLI_SUPPORT 0)
endif()

include(CMakeCompilers.txt)
include(CMakeRequiredLibraries.txt)

include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_BINARY_DIR}/src/include)

add_subdirectory(src)
if (NOT (DEFINED NO_TESTS OR NO_TESTS))
	add_subdirectory(tests)
endif()
if (NOT (DEFINED NO_TOOLS OR NO_TOOLS))
	add_subdirectory(tools)
endif()
