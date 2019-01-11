# Copyright 2018-2019 Dynatrace LLC
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
function(_set_common_flags target)

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

	_set_compiler_flags(${target} FALSE ${USE_CXX_COMPILER}) ## test_code=FALSE

    # add linker options
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

	set_target_properties( ${target}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	)

endfunction()

########################################################################################################################
# Internal utility function to set common flags on any OpenKit target
function(_set_common_flags_tests target)

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

	_set_compiler_flags(${target} TRUE ${USE_CXX_COMPILER}) ## test_code=FALSE

    # add linker options
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

	set_target_properties( ${target}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	)

endfunction()

########################################################################################################################
# Internal utility function for building an OpenKit target
function(_open_kit_build name includedirs libs)

    target_include_directories(${name} PRIVATE ${includedirs})

    # To support mixing linking in static and dynamic libraries, link each
    # library in with an extra call to TARGET_LINK_LIBRARIES.
    foreach (lib "${libs}")
        target_link_libraries(${name} PRIVATE ${lib})
    endforeach ()

    # setup some common flags
    _set_common_flags("${name}")

endfunction()

########################################################################################################################
# Function used to build a shared library.
function(open_kit_build_shared_library name includedirs libs)

    message(INFO " Configuring shared library '${name} ('INCLUDEDIRS=${includedirs}; LIBS=${libs}) ")
    add_library(${name} SHARED ${ARGN})
    
    _open_kit_build("${name}" "${includedirs}" "${libs}")
	set_property(TARGET ${name} PROPERTY POSITION_INDEPENDENT_CODE ON)

endfunction()

########################################################################################################################
# Function used to build a static library.
function(open_kit_build_static_library name includedirs libs)

    message(INFO " Configuring static library '${name}' (INCLUDEDIRS=${includedirs}; LIBS=${libs}) ")
    add_library(${name} STATIC ${ARGN})

    _open_kit_build("${name}" "${includedirs}" "${libs}")

endfunction()


########################################################################################################################
# Function to build a library - if OPEN_KIT_SHARED_LIBS is on, it's built as shared library, otherwhise as static lib.
function(open_kit_build_library name includedirs libs)
    ## check if CFLAGS or CXXFLAGS are required
    _determine_compiler_language(${name} ${ARGN})

    if (BUILD_SHARED_LIBS)
        open_kit_build_shared_library("${name}" "${includedirs}" "${libs}" ${ARGN})
    else ()
        open_kit_build_static_library("${name}" "${includedirs}" "${libs}" ${ARGN})
    endif ()

endfunction()


########################################################################################################################
# Function to build an executable
function(open_kit_build_executable name includedirs libs)

    message(INFO " Configuring executable '${name}' (INCLUDEDIRS=${includedirs}; LIBS=${libs}")

    ## check if CFLAGS or CXXFLAGS are required
    _determine_compiler_language(${name} ${ARGN})

    add_executable(${name} ${ARGN})

    _open_kit_build("${name}" "${includedirs}" "${libs}")

endfunction()

########################################################################################################################
# Function to build a test application
#
# gtest library and appropriate include directories don't need to be set up, since this is added already.
function(open_kit_build_test name includedirs libs)

    if (NOT OPENKIT_BUILD_TESTS)
        message(INFO " Tests are disabled for OpenKit project - skpipping ${name}")
        return()
    endif ()

    ## check if CFLAGS or CXXFLAGS are required
    _determine_compiler_language(${name} ${ARGN})

    message(INFO " Configuring test '${name}' (INCLUDEDIRS=${includedirs}; LIBS=${libs}")
    add_executable(${name} ${ARGN})

    # setup some additional define required by gtest, but only for Visual C++ compiler
    if (MSVC)
        target_compile_definitions(${name} PRIVATE "GTEST_LANG_CXX11=1" "GTEST_HAS_TR1_TUPLE=0")
    endif ()
	
	set (test_libs ${libs} gtest gmock gmock_main)

	target_include_directories(${name} SYSTEM PRIVATE ${gtest_SOURCE_DIR}/include)
	target_include_directories(${name} SYSTEM PRIVATE ${gmock_SOURCE_DIR}/include)
	target_include_directories(${name} PRIVATE ${includedirs})

    # To support mixing linking in static and dynamic libraries, link each
    # library in with an extra call to TARGET_LINK_LIBRARIES.
    foreach (lib "${test_libs}")
        target_link_libraries(${name} PRIVATE ${lib})
    endforeach ()

    # setup some common flags
    _set_common_flags_tests("${name}")

    # add test for CTest
    add_test(NAME ${name}
             COMMAND ${name} --gtest_output=xml:./TestResults/${name}.xml
             WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

endfunction()
