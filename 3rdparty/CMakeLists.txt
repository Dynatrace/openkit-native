# Copyright 2018 Dynatrace LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

######
# build all mandatory dependencies for OpenKit
######

######
# zlib
add_subdirectory(zlib-1.2.11)

######
# libcurl
set(BUILD_CURL_EXE OFF CACHE BOOL "Set to ON to build curl executable.")
if (BUILD_SHARED_LIBS)
	set(CURL_STATICLIB OFF CACHE BOOL "Set to ON to build libcurl with static linking.")
else()
	set(CURL_STATICLIB ON CACHE BOOL "Set to ON to build libcurl with static linking.")
	if(WIN32)
		if (OPENKIT_FORCE_SHARED_CRT)
			set(CURL_STATIC_CRT OFF CACHE BOOL "Set to ON to build libcurl with static CRT on Windows (/MT).")
		else()
			set(CURL_STATIC_CRT ON CACHE BOOL "Set to ON to build libcurl with static CRT on Windows (/MT).")
		endif()
	endif()
endif()
set(HTTP_ONLY ON CACHE BOOL "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)")
set(CMAKE_USE_LIBSSH2 OFF CACHE BOOL "Use libSSH2")
set(CURL_ZLIB ON CACHE BOOL "Set to ON to enable building curl with zlib support.")
if(WIN32)
    # on Windows use Windows native SSL/TLS implementation
	set(CMAKE_USE_WINSSL ON CACHE BOOL "enable Windows native SSL/TLS")
	set(CMAKE_USE_DARWINSSL OFF CACHE BOOL "enable Apple OS native SSL/TLS")
	set(CMAKE_USE_OPENSSL OFF CACHE BOOL "Use OpenSSL code. Experimental")
elseif(APPLE)
	# on Darwin OS (OS X/iOS/...) use their native SSL/TLS implementation
	set(CMAKE_USE_WINSSL OFF CACHE BOOL "enable Windows native SSL/TLS")
	set(CMAKE_USE_DARWINSSL ON CACHE BOOL "enable Apple OS native SSL/TLS")
	set(CMAKE_USE_OPENSSL OFF CACHE BOOL "Use OpenSSL code. Experimental")
else()
	# on all other OS fall back to OpenSSL
	set(CMAKE_USE_WINSSL OFF CACHE BOOL "enable Windows native SSL/TLS")
	set(CMAKE_USE_DARWINSSL OFF CACHE BOOL "enable Apple OS native SSL/TLS")
	set(CMAKE_USE_OPENSSL ON CACHE BOOL "Use OpenSSL code. Experimental")
endif()
add_subdirectory(curl-7.59.0)

######
# google test & google mock
if(OPENKIT_BUILD_TESTS)
	if(BUILD_SHARED_LIBS OR OPENKIT_FORCE_SHARED_CRT)
		set(gtest_force_shared_crt ON CACHE BOOL "Set to ON to build libcurl with static CRT on Windows (/MT)." FORCE)
		message("here")
	else ()
		set(gtest_force_shared_crt OFF CACHE BOOL "Set to ON to build libcurl with static CRT on Windows (/MT)." FORCE)
		message("there")
	endif()
	message("gtest_force_shared_crt = ${gtest_force_shared_crt}")
	
	add_subdirectory(googletest-release-1.8.0)
	if (MSVC)
		# hack necessary for Visual C++ compilers
		# gtest does not know that newer Visual C++ compilers
		# are C++11 compliant.
		# MS deprecated tr1 namespace with Visual C++ 17 and due to "warnings as errors"-flag
		# this leads to failing build
		set (GTEST_MSVC_PREPROCESSOR_HACKS "GTEST_LANG_CXX11=1" "GTEST_HAS_TR1_TUPLE=0")
		target_compile_definitions(gtest PUBLIC ${GTEST_MSVC_PREPROCESSOR_HACKS})
		target_compile_definitions(gtest_main PUBLIC ${GTEST_MSVC_PREPROCESSOR_HACKS})
		target_compile_definitions(gmock PUBLIC ${GTEST_MSVC_PREPROCESSOR_HACKS})
		target_compile_definitions(gmock_main PUBLIC ${GTEST_MSVC_PREPROCESSOR_HACKS})
	endif ()
endif()