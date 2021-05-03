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

# file containing some nice utility functions for building targets


##################################################################################
# internal function to determine if C-only or C++ compiler is used
# A c-only compiler uses the OPEN_KIT_C_FLAGS_... flags
# A C++ compiler used the OPEN_KIT_CXX_FLAGS_... flags
# every build unit has either C or C++ compiler(flags)
function(_determine_compiler_language target name)
	set(USING_C_COMPILER FALSE)
	set(USING_CXX_COMPILER FALSE)

	foreach( file ${ARGN})
		string(REGEX MATCH "[^.]*.cxx$"  FOUND_CXX ${ARGN} )
		if(FOUND_CXX AND NOT USING_CXX_COMPILER)
			set(USING_CXX_COMPILER TRUE)
		endif()
		string(REGEX MATCH "[^.]*.c$"  FOUND_C ${ARGN} )
		if(FOUND_C AND NOT USING_C_COMPILER)
			set(USING_C_COMPILER TRUE)
		endif()
	endforeach()

	set(USE_CXX_COMPILER ${USING_CXX_COMPILER} PARENT_SCOPE)

	if(USING_C_COMPILER AND USING_CXX_COMPILER)
		message(ERROR "file list of build target ${name} contains files of different language")
	endif()

endfunction()

##################################################################################
# internal function to ease distinction between CFLAGS and CXXFLAGS
function(_set_compiler_flags target test_code cxx_compiler)

    if(NOT test_code AND cxx_compiler)
		if (NOT OPEN_KIT_CXX_FLAGS STREQUAL "")
			target_compile_options(${target} PRIVATE ${OPEN_KIT_CXX_FLAGS})
		endif ()
		if (NOT OPEN_KIT_CXX_FLAGS_DEBUG STREQUAL "")
			target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:${OPEN_KIT_CXX_FLAGS_DEBUG}>)
		endif ()
		if (NOT OPEN_KIT_CXX_FLAGS_RELEASE STREQUAL "")
			target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:${OPEN_KIT_CXX_FLAGS_RELEASE}>)
		endif ()
	endif()

	if(test_code AND cxx_compiler)
		if (NOT OPEN_KIT_CXX_FLAGS_TESTS STREQUAL "")
			target_compile_options(${target} PRIVATE ${OPEN_KIT_CXX_FLAGS_TESTS})
		endif ()
		if (NOT OPEN_KIT_CXX_FLAGS_DEBUG STREQUAL "")
			target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:${OPEN_KIT_CXX_FLAGS_DEBUG}>)
		endif ()
		if (NOT OPEN_KIT_CXX_FLAGS_RELEASE STREQUAL "")
			target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:${OPEN_KIT_CXX_FLAGS_RELEASE}>)
		endif ()
	endif()

	if(NOT test_code AND NOT cxx_compiler)
		if (NOT OPEN_KIT_C_FLAGS STREQUAL "")
			target_compile_options(${target} PRIVATE ${OPEN_KIT_C_FLAGS})
		endif ()
		if (NOT OPEN_KIT_C_FLAGS_DEBUG STREQUAL "")
			target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:${OPEN_KIT_C_FLAGS_DEBUG}>)
		endif ()
		if (NOT OPEN_KIT_C_FLAGS_RELEASE STREQUAL "")
			target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:${OPEN_KIT_C_FLAGS_RELEASE}>)
		endif ()
	endif()

	if(test_code AND NOT cxx_compiler)
		if (NOT OPEN_KIT_C_FLAGS_TESTS STREQUAL "")
			target_compile_options(${target} PRIVATE ${OPEN_KIT_C_FLAGS_TESTS})
		endif ()
		if (NOT OPEN_KIT_C_FLAGS_DEBUG STREQUAL "")
			target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:${OPEN_KIT_C_FLAGS_DEBUG}>)
		endif ()
		if (NOT OPEN_KIT_C_FLAGS_RELEASE STREQUAL "")
			target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:${OPEN_KIT_C_FLAGS_RELEASE}>)
		endif ()
	endif()

endfunction()

########################################################################################################################
# Internal utility function to set common flags on any OpenKit target
function(_set_common_flags target IS_TEST_TARGET)

    # add preprocessor definitions
    if (NOT OPEN_KIT_PREPROCESSOR_DEFINITIONS STREQUAL "")
        target_compile_definitions(${target} PRIVATE ${OPEN_KIT_PREPROCESSOR_DEFINITIONS})
    endif ()
    if (NOT OPEN_KIT_PREPROCESSOR_DEFINITIONS_DEBUG STREQUAL "")
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:${OPEN_KIT_PREPROCESSOR_DEFINITIONS_DEBUG}>)
    endif ()
    if (NOT OPEN_KIT_PREPROCESSOR_DEFINITIONS_RELEASE STREQUAL "")
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Release>:${OPEN_KIT_PREPROCESSOR_DEFINITIONS_RELEASE}>)
    endif ()

	_set_compiler_flags(${target} ${IS_TEST_TARGET} ${USE_CXX_COMPILER})

    # add linker options
	# note: linker flags and librarian flags have different properties
	get_target_property(target_type ${target} TYPE)
	if (target_type STREQUAL "STATIC_LIBRARY")
		if (NOT OPEN_KIT_LIB_LINKER_FLAGS STREQUAL "")
			string(REPLACE ";" " " FLAGS "${OPEN_KIT_LIB_LINKER_FLAGS}")
			set_target_properties(${target} PROPERTIES STATIC_LIBRARY_FLAGS "${FLAGS}")
		endif ()
		if (NOT OPEN_KIT_LIB_LINKER_FLAGS_DEBUG STREQUAL "")
			string(REPLACE ";" " " FLAGS "${OPEN_KIT_LIB_LINKER_FLAGS_DEBUG}")
			set_target_properties(${target} PROPERTIES STATIC_LIBRARY_FLAGS_DEBUG "${FLAGS}")
		endif ()
		if (NOT OPEN_KIT_LIB_LINKER_FLAGS_RELEASE STREQUAL "")
			string(REPLACE ";" " " FLAGS "${OPEN_KIT_LIB_LINKER_FLAGS_RELEASE}")
			set_target_properties(${target} PROPERTIES STATIC_LIBRARY_FLAGS_RELEASE "${FLAGS}")
		endif ()
	else ()
		if (NOT OPEN_KIT_LINKER_FLAGS STREQUAL "")
			string(REPLACE ";" " " FLAGS "${OPEN_KIT_LINKER_FLAGS}")
			set_target_properties(${target} PROPERTIES LINK_FLAGS "${FLAGS}")
		endif ()
		if (NOT OPEN_KIT_LINKER_FLAGS_DEBUG STREQUAL "")
			string(REPLACE ";" " " FLAGS "${OPEN_KIT_LINKER_FLAGS_DEBUG}")
			set_target_properties(${target} PROPERTIES LINK_FLAGS_DEBUG "${FLAGS}")
		endif ()
		if (NOT OPEN_KIT_LINKER_FLAGS_RELEASE STREQUAL "")
			string(REPLACE ";" " " FLAGS "${OPEN_KIT_LINKER_FLAGS_RELEASE}")
			set_target_properties(${target} PROPERTIES LINK_FLAGS_RELEASE "${FLAGS}")
		endif ()
	endif ()

	set_target_properties( ${target}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	)

    # enforce C++11 standard by default in every of our targets
    enforce_cxx11_standard(${target})

endfunction()


########################################################################################################################
# Function to add a library target - if BUILD_SHARED_LIBS is on, it's added as shared library, otherwhise as static lib.
# An alias target with Dynatrace:: prefix is created also.
#
# This also sets up common compiler flags that are used throughout the whole OpenKit build.
function(open_kit_add_library name)
    ## check if CFLAGS or CXXFLAGS are required
    _determine_compiler_language(${name} ${ARGN})

    if (BUILD_SHARED_LIBS)
        message(INFO " Configuring shared library '${name}")
        add_library(${name} SHARED ${ARGN})
    else ()
        message(INFO " Configuring static library '${name}'")
        add_library(${name} STATIC ${ARGN})
    endif ()

    # add aliased target
    add_library(
        Dynatrace::${name}
        ALIAS ${name}
    )

    # setup some common flags
    _set_common_flags(${name} FALSE)

endfunction()

########################################################################################################################
# Function to add an executable target
#
# This also sets up common compiler flags that are used throughout the whole OpenKit build.
function(open_kit_add_executable name)

    message(INFO " Configuring executable '${name}'")

    ## check if CFLAGS or CXXFLAGS are required
    _determine_compiler_language(${name} ${ARGN})

    add_executable(${name} ${ARGN})

    # setup some common flags
    _set_common_flags(${name} FALSE)

endfunction()

########################################################################################################################
# Function to build add a test executable target
#
# gtest library and appropriate include directories don't need to be set up, since this is added already.
function(open_kit_add_test name)

    if (NOT OPENKIT_BUILD_TESTS)
        message(INFO " Tests are disabled for OpenKit project - skpipping ${name}")
        return()
    endif ()

    ## check if CFLAGS or CXXFLAGS are required
    _determine_compiler_language(${name} ${ARGN})

    message(INFO " Configuring test '${name}'")
    add_executable(${name} ${ARGN})

	# add gmock_main library as testing dependency
	target_link_libraries(${name} PRIVATE gmock_main)

    # setup some common flags
    _set_common_flags("${name}" TRUE)

    # add test for CTest
    add_test(NAME ${name}
             COMMAND ${name} --gtest_output=xml:./TestResults/${name}.xml
             WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

endfunction()
