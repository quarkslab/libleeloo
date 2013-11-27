# Locate libleeloo include paths and libraries

# This module defines
# LIBLEELOO_INCLUDE_DIRS, where to find the leeloo include directory
# LIBLEELOO_LIBRARY_DIRS, where to find libleeloo.so
# LIBLEELOO_LIBRARIES, where to find libleeloo.so

# LIBLEELOO_FOUND, If false, don't try to use libleeloo.
set (LEELOO_FOUND "NO")

find_path(LIBLEELOO_INCLUDE_DIRS leeloo/list_intervals.h HINTS /usr/include /usr/local/include)
find_library(LIBLEELOO_LIBRARY_DIRS leeloo/list_intervals.h HINTS /usr/include /usr/local/include)
