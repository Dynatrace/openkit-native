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


# This file contains the build options which can be set via
# CMake by the developer using OpenKit.


# Option enabling or disableing building and running of unit tests
option(OPENKIT_BUILD_TESTS "Build tests (default: ON)" ON)

# option to build API documentation via Doxygen
option(BUILD_DOC  "Create and install the HTML based API documentation (requires Doxygen)" OFF)

# build OpenKit as static or shared library
option(BUILD_SHARED_LIBS "Build Shared Libraries" OFF)

# When other libraries are using a shared version of runtime libraries, OpenKit also has to use one.
option(OPENKIT_FORCE_SHARED_CRT "Use shared (DLL) run-time lib even when OpenKit is built as static lib." OFF)

# Set the paths where the executable, libraries and header paths
set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Installation directory for headers")
