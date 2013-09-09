set(PYTHON_SRC_FILES
	pyleeloo.cpp
)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${TBB_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

add_library(pyleeloo SHARED ${PYTHON_SRC_FILES})
set_target_properties(pyleeloo PROPERTIES PREFIX "") # We need our ".so" to be named "pyleeloo" (same name as in the BOOST_PYTHON_MODULE macro)
target_link_libraries(pyleeloo leeloo ${PYTHON_LIBRARIES} ${Boost_LIBRARIES} ${TBB_LIBRARIES})

execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())" OUTPUT_VARIABLE PYTHON_INSTALL OUTPUT_STRIP_TRAILING_WHITESPACE)
install(TARGETS pyleeloo LIBRARY DESTINATION ${PYTHON_INSTALL}/)

#if (PYTHONINTERP_FOUND)
#	set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
#	set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
#	set(DEPS        "${CMAKE_CURRENT_SOURCE_DIR}/pyleeloo.cpp")
#	set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build")
#
#	configure_file(${SETUP_PY_IN} ${SETUP_PY})
#
#	add_custom_command(OUTPUT ${OUTPUT}
#		COMMAND ${PYTHON_EXECUTABLE}
#		ARGS setup.py build
#		DEPENDS ${DEPS})
#
#	add_custom_target(target ALL DEPENDS ${OUTPUT})
#
#	install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SETUP_PY} install)")
#endif()
