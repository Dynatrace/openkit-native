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

SET(OPENKIT_SAMPLE_CXX_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/sample1/src/CommandLineArguments.cxx
    ${CMAKE_CURRENT_LIST_DIR}/sample1/src/CommandLineArguments.h
    ${CMAKE_CURRENT_LIST_DIR}/sample1/src/openkit-sample.cxx
)

SET(OPENKIT_SAMPLE_C_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/sample2/src/openkit-sample.c
)

include(CompilerConfiguration)
fix_compiler_flags()

function(_build_sample_internal target)

    include(CompilerConfiguration)
    include(BuildFunctions)

    open_kit_add_executable(${target} ${ARGN})
    target_link_libraries(${target} PRIVATE Dynatrace::OpenKit)

    if (WIN32 AND BUILD_SHARED_LIBS)
       add_custom_command ( TARGET ${target} POST_BUILD 
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:OpenKit> $<TARGET_FILE_DIR:${target}> )

        # it not monolithic shared library
        if (NOT OPENKIT_MONOLITHIC_SHARED_LIB) 
            add_custom_command ( TARGET ${target} POST_BUILD 
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:zlib> $<TARGET_FILE_DIR:${target}> 
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libcurl> $<TARGET_FILE_DIR:${target}>  )
        endif()
    endif()
endfunction()

function(build_cxx_sample)
    message("Configuring OpenKit sample (C++) ... ")

    _build_sample_internal(openkit-sample ${OPENKIT_SAMPLE_CXX_SOURCES})
    source_group("Source Files" FILES ${OPENKIT_SAMPLE_CXX_SOURCES})
endfunction()

function(build_c_sample2)
    message("Configuring OpenKit sample (C) ... ")

    _build_sample_internal(openkit-sample-c ${OPENKIT_SAMPLE_C_SOURCES})
    source_group("Source Files" FILES ${OPENKIT_SAMPLE_C_SOURCES})
endfunction()

function(build_open_kit_samples)
    build_cxx_sample()
    build_c_sample2()
    set_target_properties(openkit-sample PROPERTIES FOLDER Samples)
    set_target_properties(openkit-sample-c PROPERTIES FOLDER Samples)
endfunction()
