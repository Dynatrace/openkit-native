# Copyright 2018-2021 Dynatrace LLC
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

##
# This is a workaround to ensure when OpenKit is built using static CRT,
# then all 3rd party libs must use static CRT as well
# Pass USE_DEFAULT_WARNINGS_FLAGS to ensure the default warning flags are not overridden (MSVC only).
include(CompilerConfiguration)
fix_compiler_flags(USE_DEFAULT_WARNINGS_FLAGS)

######
# zlib
SET(SKIP_INSTALL_FILES TRUE)
set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
if (NOT BUILD_SHARED_LIBS OR OPENKIT_MONOLITHIC_SHARED_LIB)
    set(BUILD_SHARED_LIBS OFF)
endif()
add_subdirectory(zlib-1.3)
set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")

######
# libcurl
set(BUILD_CURL_EXE OFF CACHE BOOL "Set to ON to build curl executable.")
set(CURL_DISABLE_TESTS OFF CACHE BOOL "Disable building cURL tests.")
if(MSVC)
	if (OPENKIT_FORCE_SHARED_CRT OR (OPENKIT_MONOLITHIC_SHARED_LIB AND NOT OPENKIT_FORCE_STATIC_CRT))
		set(CURL_STATIC_CRT OFF CACHE BOOL "Set to ON to build libcurl with static CRT on Windows (/MT).")
	else()
		set(CURL_STATIC_CRT ON CACHE BOOL "Set to ON to build libcurl with static CRT on Windows (/MT).")
	endif()
endif()

set(HTTP_ONLY ON CACHE BOOL "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)")
set(CMAKE_USE_LIBSSH2 OFF CACHE BOOL "Use libSSH2")
set(CURL_ZLIB ON CACHE BOOL "Set to ON to enable building curl with zlib support.")
if(WIN32)
    # on Windows use Windows native SSL/TLS implementation
	set(CMAKE_USE_SCHANNEL ON CACHE BOOL "enable Windows native SSL/TLS")
	set(CMAKE_USE_SECTRANSP OFF CACHE BOOL "enable Apple OS native SSL/TLS")
	set(CMAKE_USE_OPENSSL OFF CACHE BOOL "Use OpenSSL code. Experimental")
elseif(APPLE)
	# on Darwin OS (OS X/iOS/...) use their native SSL/TLS implementation
	set(CMAKE_USE_SCHANNEL OFF CACHE BOOL "enable Windows native SSL/TLS")
	set(CMAKE_USE_SECTRANSP ON CACHE BOOL "enable Apple OS native SSL/TLS")
	set(CMAKE_USE_OPENSSL OFF CACHE BOOL "Use OpenSSL code. Experimental")
else()
	# on all other OS fall back to OpenSSL
	set(CMAKE_USE_SCHANNEL OFF CACHE BOOL "enable Windows native SSL/TLS")
	set(CMAKE_USE_SECTRANSP OFF CACHE BOOL "enable Apple OS native SSL/TLS")
	set(CMAKE_USE_OPENSSL ON CACHE BOOL "Use OpenSSL code. Experimental")
endif()

set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
if (NOT BUILD_SHARED_LIBS OR OPENKIT_MONOLITHIC_SHARED_LIB)
    set(BUILD_SHARED_LIBS OFF)
endif()
add_subdirectory(curl-8.4.0)
set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")

######
# google test & google mock
if(OPENKIT_BUILD_TESTS)
    # enforce same shared CRT settings as for OpenKit
	if(BUILD_SHARED_LIBS OR OPENKIT_FORCE_SHARED_CRT)
		set(gtest_force_shared_crt ON CACHE BOOL "Set to ON to build googletest with static CRT on Windows (/MT)." FORCE)
	else ()
		set(gtest_force_shared_crt OFF CACHE BOOL "Set to ON to build googletest with static CRT on Windows (/MT)." FORCE)
	endif()

	# enforce building gmock, which includes gtest as well
	set(BUILD_GMOCK ON CACHE BOOL "Builds the googlemock subproject" FORCE)

	# disable installation of Google test
	set(INSTALL_GTEST OFF CACHE BOOL "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" FORCE)
	
	# build googletest always as static library
	# by temporarily turning of the BUILD_SHARED_LIBS
	set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(googletest-release-1.12.1)
    set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")
	
endif()

set_target_properties(gmock PROPERTIES FOLDER 3rdparty/googletest)
set_target_properties(gmock_main PROPERTIES FOLDER 3rdparty/googletest)
set_target_properties(gtest PROPERTIES FOLDER 3rdparty/googletest)
set_target_properties(gtest_main PROPERTIES FOLDER 3rdparty/googletest)
if (BUILD_SHARED_LIBS AND NOT OPENKIT_MONOLITHIC_SHARED_LIB)
	set_target_properties(zlib PROPERTIES FOLDER 3rdparty/zlib)
else ()
	set_target_properties(zlibstatic PROPERTIES FOLDER 3rdparty/zlib)
endif ()
