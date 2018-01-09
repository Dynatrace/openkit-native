cmake_minimum_required(VERSION 2.6)

project(openkit-c)

if(WIN32)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W4")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")
endif()
########################################
# dependencies
########################################
if(WIN32)
	find_package(CURL REQUIRED PATHS ${CMAKE_SOURCE_DIR}/3rdparty/CURL/)
else()
	find_package(CURL)
endif()

message("CURL Headers: '${CURL_INCLUDE_DIRS}'")
message("CURL LIB: '${CURL_LIBRARIES}'")

if(WIN32)
	set(ZLIB_LIBRARIES ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.3/dll32/zlibwapi.lib)
	set(ZLIB_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.3/include)
else()
	find_package(ZLIB)
endif()

include_directories(${CURL_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/openkit-c/include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_subdirectory("openkit-c")
