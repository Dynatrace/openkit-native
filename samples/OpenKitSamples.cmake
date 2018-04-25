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

SET(OPENKIT_SAMPLE1_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/sample1/src/openkit-sample.cxx
	${CMAKE_CURRENT_LIST_DIR}/sample1/src/CommandLineArguments.cxx
)

SET(OPENKIT_SAMPLE2_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/sample2/src/openkit-sample.c
)

include(CompilerConfiguration)
fix_compiler_flags()

function(_build_sample_internal target)
	find_package(ZLIB)
	find_package(CURL)


	set(SAMPLE_INCLUDE_DIRS
		${ZLIB_INCLUDE_DIR}
		${CURL_INCLUDE_DIR}
		${OpenKit_SOURCE_DIR}/include # TODO stefan.eberl - include from <OpenKit/OpenKit.h>
		${OpenKit_BINARY_DIR}/include
	)

	set(SAMPLE_LIBS
		${ZLIB_LIBRARY}
		${CURL_LIBRARY}
		OpenKit
	)

	include(CompilerConfiguration)
	include(BuildFunctions)

	open_kit_build_executable(${target} "${SAMPLE_INCLUDE_DIRS}" "${SAMPLE_LIBS}" ${ARGN})
	enforce_cxx11_standard(${target})
	if (NOT BUILD_SHARED_LIBS)
		target_compile_definitions(${target} PRIVATE -DCURL_STATICLIB)
	endif()

	if (WIN32)
	   add_custom_command ( TARGET ${target} POST_BUILD 
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:OpenKit> $<TARGET_FILE_DIR:${target}> 
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:zlib> $<TARGET_FILE_DIR:${target}> 
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libcurl> $<TARGET_FILE_DIR:${target}>  )
	endif()
endfunction()

function(build_sample1)
	message("Configuring OpenKit  sample 1 ... ")

	_build_sample_internal(openkit-sample ${OPENKIT_SAMPLE1_SOURCES})

endfunction()

function(build_sample2)
	message("Configuring OpenKit  sample 1 ... ")

	_build_sample_internal(openkit-sample-c ${OPENKIT_SAMPLE2_SOURCES})
endfunction()

function(build_open_kit_samples)
	build_sample1()
	build_sample2()
endfunction()
