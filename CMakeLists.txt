cmake_minimum_required(VERSION 2.8)

project(openkit-c)

option(OPENKIT_BUILD_TESTS "Build tests" ON )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(UtilMacros)
fix_default_compiler_settings_()
check_for_cxx_version_()

if(NOT WIN32)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic -std=c++0x -pthread")
endif()

########################################
# dependencies
########################################
if(WIN32)
	find_package(CURL REQUIRED PATHS ${CMAKE_SOURCE_DIR}/3rdparty/curl-7.57.0/)
	string(REPLACE "bin/libcurl.dll" "lib/libcurl_imp.lib" CURLIMPLIB ${CURL_LIBRARIES})##FindCURL does not offer the implib directly
	if(NOT EXISTS ${CURLIMPLIB})
		message(SEND_ERROR "implib of curl not found.")
	endif()
else()
	find_package(CURL REQUIRED)
    set(CURLIMPLIB ${CURL_LIBRARIES})
endif()

message("CURL Headers: '${CURL_INCLUDE_DIRS}'")
message("CURL LIB: '${CURL_LIBRARIES}'")

if(WIN32)
	set(ZLIB_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11)
	find_package(ZLIB REQUIRED)
endif()

find_package(ZLIB REQUIRED)
message("ZLIB Headers: '${ZLIB_INCLUDE_DIRS}'")
message("ZLIB LIB: '${ZLIB_LIBRARIES}'")

if(OPENKIT_BUILD_TESTS)
	###googletest 1.8.0 used in Visual Studio 2017 leads to a deprecation warning
	### in the tr1::tuple namespace
	if(MSVC)
		add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING )
	endif()

	if(WIN32)
		if(${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
			set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/googletest-1.8.0/win32-release/)
		else()
			set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/googletest-1.8.0/win32-debug/)
		endif()
	else()
		set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/googletest-1.8.0/linux/)
	endif()

	find_package(GTest REQUIRED)

	enable_testing()

	message("GTEST Headers: '${GTEST_INCLUDE_DIRS}'")
	message("GTEST LIB: '${GTEST_LIBRARIES}'")
endif()

include_directories(${CURL_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory("src")
add_subdirectory("test")
#add_subdirectory("sample")
