# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# This file has been taken from CMake's distributed FindCURL.cmake
# and has been adjusted for OpenKit

# Look for the header file.
find_path(CURL_INCLUDE_DIR
		  NAMES curl/curl.h
		  PATHS "${CURL_SOURCE_DIR}/include"
		  NO_DEFAULT_PATH)

# don't look for libcurl, since it will be built later (as dependency)
# just set the appropriate value
set(CURL_LIBRARY libcurl)

if(CURL_INCLUDE_DIR)
  foreach(_curl_version_header curlver.h curl.h)
    if(EXISTS "${CURL_INCLUDE_DIR}/curl/${_curl_version_header}")
      file(STRINGS "${CURL_INCLUDE_DIR}/curl/${_curl_version_header}" curl_version_str REGEX "^#define[\t ]+LIBCURL_VERSION[\t ]+\".*\"")

      string(REGEX REPLACE "^#define[\t ]+LIBCURL_VERSION[\t ]+\"([^\"]*)\".*" "\\1" CURL_VERSION_STRING "${curl_version_str}")
      string(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*$" "\\1" CURL_VERSION_MAJOR "${CURL_VERSION_STRING}")
      string(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+.*$" "\\1" CURL_VERSION_MINOR  "${CURL_VERSION_STRING}")
      string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" CURL_VERSION_PATCH "${CURL_VERSION_STRING}")
      unset(curl_version_str)
      break()
    endif()
  endforeach()
endif()

# set all variables that would normally be set by find_package_handle_standard_args
set(CURL_FOUND TRUE)
set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIR})
set(CURL_LIBRARIES ${CURL_LIBRARY})