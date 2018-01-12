cmake_minimum_required(VERSION 2.6)

project(openkit-c)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(UtilMacros)
fix_default_compiler_settings_()

if(WIN32)
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic -std=c99 -pthread")
	add_definitions(-D_XOPEN_SOURCE=500)
endif()
########################################
# dependencies
########################################
if(WIN32)
	find_package(CURL REQUIRED PATHS ${CMAKE_SOURCE_DIR}/3rdparty/curl-7.57.0/)
else()
	find_package(CURL)
endif()

message("CURL Headers: '${CURL_INCLUDE_DIRS}'")
message("CURL LIB: '${CURL_LIBRARIES}'")

if(WIN32)
	set(ZLIB_LIBRARIES ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/lib/zlibd.lib)
	set(ZLIB_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/include)
else()
	find_package(ZLIB)
endif()

if(WIN32)
	set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/googletest-1.8.0/win32/)
else()
	set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/googletest-1.8.0/linux/)
endif()

find_package(GTest REQUIRED)

include_directories(${CURL_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/openkit-c/include)
include_directories(${CMAKE_SOURCE_DIR}/openkit-c/src/base)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_subdirectory("openkit-c")
