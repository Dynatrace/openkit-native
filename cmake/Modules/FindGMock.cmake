# File downloaded with minor changes from https://github.com/facebook/fbthrift/blob/master/thrift/cmake/FindGMock.cmake
# fbthrift is licensed under Apache License 2.0

# Find GMock
#
# This will define:
# GMOCK_FOUND
# GMOCK_INCLUDE_DIRS
# GMOCK_LIBRARIES
# GMOCK_MAIN_LIBRARIES
# GMOCK_BOTH_LIBRARIES

find_path(GMOCK_INCLUDE_DIRS gmock/gmock.h
    HINTS
        $ENV{GMOCK_ROOT}/include
        ${GMOCK_ROOT}/include
)

find_library(GMOCK_LIBRARIES
    NAMES gmock
    HINTS
        $ENV{GMOCK_ROOT}
        ${GMOCK_ROOT}
	PATH_SUFFIXES "lib"
)

find_library(GMOCK_MAIN_LIBRARIES
    NAMES gmock_main
    HINTS
        $ENV{GMOCK_ROOT}
        ${GMOCK_ROOT}
	PATH_SUFFIXES "lib"
)
set(GMOCK_BOTH_LIBRARIES ${GMOCK_LIBRARIES} ${GMOCK_MAIN_LIBRARIES})

mark_as_advanced(GMOCK_INCLUDE_DIRS GMOCK_LIBRARIES GMOCK_MAIN_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    GMock GMOCK_LIBRARIES GMOCK_INCLUDE_DIRS GMOCK_MAIN_LIBRARIES)

if(GMOCK_FOUND AND NOT GMOCK_FIND_QUIETLY)
    message(STATUS "GMOCK: ${GMOCK_INCLUDE_DIRS}")
endif()
