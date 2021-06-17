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

include (CheckCXXSourceCompiles)
include (CheckLibraryExists)

########################################################################################################################
# Check if atomic operations work without explicitely linking to libatomic
function (check_for_working_atomic_ops var)

    # older versions of CMake do not handle set required C++ standard
    # append appropriate flag to the compiler and hope 
    set (OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
    set (CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")
    CHECK_CXX_SOURCE_COMPILES("
        #include <atomic>
        #include <cstdint>

        std::atomic<std::int64_t> x(0);
        int main(int, char**)
        {
            return x;
        }
    "
    ${var})
    set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})

endfunction()

if(MSVC)
  # No need to check with MSVC compiler
  set (HAVE_CXX_ATOMICS_WITHOUT_LIB TRUE)
else()
  # First check if atomics work without the library.
  check_for_working_atomic_ops(HAVE_CXX_ATOMICS_WITHOUT_LIB)
endif()

if (NOT HAVE_CXX_ATOMICS_WITHOUT_LIB)
    check_library_exists(atomic __atomic_load_8 "" HAVE_LIBATOMIC)
    if (NOT HAVE_LIBATOMIC)
        message(STATUS "Host compiler appears to require libatomic, but cannot locate it.")
    endif ()

    list(APPEND CMAKE_REQUIRED_LIBRARIES "atomic")
    check_for_working_atomic_ops(HAVE_CXX_ATOMICS_WITH_LIB)
    if (NOT HAVE_CXX_ATOMICS_WITH_LIB)
        message(FATAL_ERROR "std::atomic is required by OpenKit, but not supported by Host compiler")
    endif ()
endif ()
