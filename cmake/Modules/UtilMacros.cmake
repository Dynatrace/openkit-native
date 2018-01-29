macro(fix_default_compiler_settings_)
  if (MSVC)
    # For MSVC, CMake sets certain flags to defaults we want to override.
    # This replacement code is taken from sample in the CMake Wiki at
    # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
    foreach (flag_var
             CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
             CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
			 CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
             CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
			 )
      # We prefer more strict warning checking for building
      # Replaces /W3 with /W4 in defaults.
      string(REPLACE "/W3" "/W4" ${flag_var} "${${flag_var}}")
    endforeach()
  endif()
endmacro()

macro(check_for_cxx_version_)
	include(CheckCXXCompilerFlag)
	##beginning with CMake version 3.1 this check would be possible with 
	#set_property(TARGET prog PROPERTY CXX_STANDARD 11)
	#set_property(TARGET prog PROPERTY CXX_STANDARD_REQUIRED ON)
	if(MSVC)
		CHECK_CXX_COMPILER_FLAG("/std:c++14" COMPILER_SUPPORTS_CXX14_VC)#no c++11 flag for Visual C++ compiler
		if(COMPILER_SUPPORTS_CXX14_VC)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14")
		else()
				message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
		endif()
	else()
		CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
		CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
		if(COMPILER_SUPPORTS_CXX11)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
		elseif(COMPILER_SUPPORTS_CXX0X)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
		else()
				message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
		endif()
	endif()
endmacro()