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


# This file contains the build options which can be set via
# CMake by the developer using OpenKit.
include (CMakeDependentOption)


# Option enabling or disableing building and running of unit tests
option(OPENKIT_BUILD_TESTS "Build tests (default: ON)" ON)

# option to build API documentation via Doxygen
option(BUILD_DOC "Create and install the HTML based API documentation (requires Doxygen)" OFF)

# build OpenKit as static or shared library
option(BUILD_SHARED_LIBS "Build Shared Libraries" OFF)

# build OpenKit with all direct dependencies included in the dll
CMAKE_DEPENDENT_OPTION(OPENKIT_MONOLITHIC_SHARED_LIB "Build monolithic OpenKit DLL with dependencies included" ON
                       "BUILD_SHARED_LIBS" OFF)

if(MSVC)
	# When other libraries are using a shared version of runtime libraries, OpenKit also has to use one.
	# This only needs to be configured correctly if and only if OpenKit is built as static library (NOT shared library)
	CMAKE_DEPENDENT_OPTION(OPENKIT_FORCE_SHARED_CRT "Use shared (DLL) run-time lib even when OpenKit is built as static lib." ON
						   "NOT BUILD_SHARED_LIBS" OFF)

	# Link C-Runtime static, even when building as dll
	# This only has effect on windows platforms
	CMAKE_DEPENDENT_OPTION(OPENKIT_FORCE_STATIC_CRT "Use static run-time lib even when OpenKit is built as dynamic lib." OFF
						   "BUILD_SHARED_LIBS" OFF)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
	OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"
	OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
endif()

# Only x64 builds are allowed
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Please switch to x64 build.")
    return()
endif()

# Set the paths where the executable, libraries and header paths
if ("${CMAKE_VERSION}" VERSION_LESS "3.7.1")
	# overwrite since the default is system directory
	set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "OpenKit default value for old CMake" FORCE)
else ()
	if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		# overwrite the default value initialized by CMake
		set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "OpenKit default value since not provided by user" FORCE)
	endif ()
endif ()
