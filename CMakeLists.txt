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

# Ensure appropriate CMake version
cmake_minimum_required(VERSION 3.0.2)

# CMake specific policies
if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# limit configurations to Debug & Release builds only
# and enable CTest
set (CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)

# project setup
project(OpenKit 
        LANGUAGES C CXX)

# set CMake search path
set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/"
    CMAKE_MODULE_PATH)

# ensure build is executed in separate directory
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are unsupported. Use another directory, like build/, "
        "as your CMake build directory.")
endif()

# build options & settings
include (BuildConfiguration)
include (CompilerConfiguration)

# version and other build information
include (OpenKitVersion)

# build 3rd party dependencies
add_subdirectory(3rdparty)

# build OpenKit sources
include(${CMAKE_CURRENT_SOURCE_DIR}/src/OpenKit.cmake)
build_open_kit()

# build OpenKit tests
if (OPENKIT_BUILD_TESTS)
    enable_testing()
    include(${CMAKE_CURRENT_SOURCE_DIR}/test/OpenKitTests.cmake)
    build_open_kit_tests()
endif()

# build samples
include(${CMAKE_CURRENT_SOURCE_DIR}/samples/OpenKitSamples.cmake)
build_open_kit_samples()

# add doc target (Doxygen)
if (BUILD_DOC)
    include(BuildDoxygenTarget)
endif()
