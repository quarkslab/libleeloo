cmake_minimum_required(VERSION 2.8)
project(pyleeloo)

include(CMakeRequiredLibraries.txt)
include(CMakeCompilers.txt)

set(PYTHON_SRC_FILES
	pyleeloo.cpp
)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${TBB_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${LIBLEELOO_INCLUDE_DIRS})

add_library(pyleeloo SHARED ${PYTHON_SRC_FILES})
set_target_properties(pyleeloo PROPERTIES PREFIX "") # We need our ".so" to be named "pyleeloo" (same name as in the BOOST_PYTHON_MODULE macro)
target_link_libraries(pyleeloo ${LIBLEELOO_LIBRARIES} ${PYTHON_LIBRARIES} ${Boost_LIBRARIES} ${TBB_LIBRARIES})

execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())" OUTPUT_VARIABLE PYTHON_INSTALL OUTPUT_STRIP_TRAILING_WHITESPACE)
install(TARGETS pyleeloo LIBRARY DESTINATION ${PYTHON_INSTALL}/)
