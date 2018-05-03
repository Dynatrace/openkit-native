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

# check for the compilers we officially support
if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"  # LLVM Clang
		OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" # Apple Clang
		OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU" # GNU Compiler Collection
		OR CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")) # Microsoft Visual C++
	 
	 message(WARNING "\nOpenKit has not yet been tested with compiler ${CMAKE_CXX_COMPILER_ID}.\n")
endif()

# setup a default set of compiler flags
if(MSVC)

    # setup preprocessor flags
    set(OPEN_KIT_PREPROCESSOR_DEFINITIONS _UNICODE UNICODE WIN32 _WIN32 NOMINMAX)
    set(OPEN_KIT_PREPROCESSOR_DEFINITIONS_DEBUG _DEBUG)
    set(OPEN_KIT_PREPROCESSOR_DEFINITIONS_RELEASE NDEBUG)
    # setup compiler flags
    set(OPEN_KIT_CXX_FLAGS /W4 /nologo /wd4251 )
    set(OPEN_KIT_CXX_FLAGS_DEBUG /Zi /Od /Ob0 /Oi /GF- /Gm /Gy /RTC1)
    set(OPEN_KIT_CXX_FLAGS_RELEASE /Zi /O2 /Ob2 /Oi /Ot /GF /Gm-)

    # setup linker flags
    set(OPEN_KIT_LINKER_FLAGS /NOLOGO)
    set(OPEN_KIT_LINKER_FLAGS_DEBUG /DEBUG)
    set(OPEN_KIT_LINKER_FLAGS_RELEASE /LTCG)
 else()
    # setup preprocessor flags
    set(OPEN_KIT_PREPROCESSOR_DEFINITIONS )
    set(OPEN_KIT_PREPROCESSOR_DEFINITIONS_DEBUG _DEBUG)
    set(OPEN_KIT_PREPROCESSOR_DEFINITIONS_RELEASE NDEBUG)

    # setup compiler flags
    set(OPEN_KIT_CXX_FLAGS -Wall -Wextra -pedantic -Weffc++)
	set(OPEN_KIT_CXX_FLAGS_TESTS  -Wall -Wextra -pedantic)
    set(OPEN_KIT_CXX_FLAGS_DEBUG -O0 -g)
    set(OPEN_KIT_CXX_FLAGS_RELEASE -O2 -g)

    # setup linker flags
    set(OPEN_KIT_LINKER_FLAGS )
    set(OPEN_KIT_LINKER_FLAGS_DEBUG )
    set(OPEN_KIT_LINKER_FLAGS_RELEASE )
endif()

########################################
# utility macro to enforce C++11 mode
macro (enforce_cxx11_standard target)
	if (CMAKE_VERSION VERSION_LESS "3.1")
		# CMake does not support the C++11 standard in this vesion
		# manually check for appropriate compiler flags
		include(CheckCXXCompilerFlag)
	
		if(MSVC)
			# Starting with Visual Studio 2015 SP 3, the /std flag has been introduced
			# Previous MSVC version
			CHECK_CXX_COMPILER_FLAG("/std:c++14" COMPILER_SUPPORTS_CXX14_VC)#no c++11 flag for Visual C++ compiler
			if(COMPILER_SUPPORTS_CXX14_VC)
				target_compile_options(${target} PUBLIC "/std:c++14")
			else()
				message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
			endif()
		else()
			# assuming it's GCC or Clang
			CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
			if(COMPILER_SUPPORTS_CXX11)
				target_compile_options(${target} PUBLIC "-std=c++11")
			else()
				message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
			endif()
		endif()
	else()
		# starting with CMake 3.1 it's quite easy to enforce a specific C++ standard
		set_property(TARGET ${target} PROPERTY CXX_STANDARD 11)
		set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON)
		set_property(TARGET ${target} PROPERTY CXX_EXTENSIONS OFF)
	endif()
endmacro()

###################################################
# utility macro to overwrite default compiler flags
macro(fix_compiler_flags)
	if (MSVC)
		# For MSVC, CMake sets certain flags to defaults we want to override.
		# This replacement code is taken from sample in the CMake Wiki at
		# http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
		foreach (flag_var
				 CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
				 CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE)
			if (NOT BUILD_SHARED_LIBS AND NOT OPENKIT_FORCE_SHARED_CRT)
				# When OpenKit is built as a shared library, it should also use shared runtime libraries.
				if(${flag_var} MATCHES "/MD")
					string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
				endif()
				if(${flag_var} MATCHES "/MDd")
					string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
				endif()
			endif()
			# We prefer more strict warning checking for building
			# Replaces /W3 with /W4 in defaults.
			string(REPLACE "/W3" "/W4" ${flag_var} "${${flag_var}}")
		endforeach()
	endif()
endmacro()
