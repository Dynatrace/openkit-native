cmake_minimum_required(VERSION 2.8)

project(openkit-c)

option(OPENKIT_BUILD_TESTS "Build tests" ON )

# Check if Doxygen is installed. If installed, provide a documentation build as an option for the user
find_package(Doxygen)
option(BUILD_DOC  "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(UtilMacros)
fix_default_compiler_settings_()
check_for_cxx_version_()

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
	set(GMOCK_ROOT ${GTEST_ROOT})
	find_package(GMock REQUIRED)

	enable_testing()

	message("GTEST Headers: '${GTEST_INCLUDE_DIRS}'")
	message("GTEST LIB: '${GTEST_LIBRARIES}'")
	message("GMOCK Headers: '${GMOCK_INCLUDE_DIRS}'")
	message("GMOCK LIB: '${GMOCK_LIBRARIES}'")
endif()

if(BUILD_DOC)
	if(NOT DOXYGEN_FOUND)
		 message(WARNING "Doxygen is needed to build the documentation.")
	endif()

	# Copy the "Doxyfile.in" to "Doxyfile", thereby replacing the variables of the input file with their values determined by CMake
	set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
	set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

	# Be sure doxygen output directory exists
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)

	message("Doxygen build started.")

	# Add the "docs" build target (build with "make docs")
	add_custom_target(docs
					  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
					  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
					  COMMENT "Generating API documentation with Doxygen"
					  VERBATIM)
endif()

include_directories(SYSTEM ${CURL_INCLUDE_DIRS})
include_directories(SYSTEM ${ZLIB_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory("src")
add_subdirectory("test")
add_subdirectory("sample")
