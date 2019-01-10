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

# file setting up OpenKit version constants

if(__OPEN_KIT_VERSION_CMAKE)
  # prevent double inclusion by testing, if this file has been included previously
  # this works similar to an include guard in C/C++
  return()
endif()
set(__OPEN_KIT_VERSION_CMAKE TRUE)

# Parse OpenKit version from version.properties file
set(VERSION_PROPERTIES_FILE "version.properties")
file(STRINGS "${VERSION_PROPERTIES_FILE}" VERSION_LINE REGEX [[^version=]] LIMIT_COUNT 1)

# content of VERSION_LINE is now version=<major>.<minor>.<bugfix>
# parse <major>, <minor> and <bugfix> into separate variables
string(REGEX MATCH "^version=[0-9]+\\.[0-9]+\\.[0-9]+.*$"  VERSION_MATCH "${VERSION_LINE}")
if (NOT VERSION_MATCH)
    # content is not in expected format
	message(FATAL_ERROR "Version string read is not in expected format.")
endif()
string(REGEX REPLACE "^version=(.*)$" "\\1" VERSION_STRING "${VERSION_LINE}")
string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+).*$" "\\1" OPENKIT_MAJOR_VERSION "${VERSION_STRING}")
string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+).*$" "\\2" OPENKIT_MINOR_VERSION "${VERSION_STRING}")
string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+).*$" "\\3" OPENKIT_BUGFIX_VERSION "${VERSION_STRING}")

if (NOT ("$ENV{TRAVIS_BUILD_NUMBER}" STREQUAL ""))
  # take build number from Travis CI
  set (OPENKIT_BUILD_VERSION $ENV{TRAVIS_BUILD_NUMBER})
elseif (NOT ("$ENV{APPVEYOR_BUILD_NUMBER}" STREQUAL ""))
  # take build number from AppVeyor
  set (OPENKIT_BUILD_VERSION $ENV{APPVEYOR_BUILD_NUMBER})
else ()
  # manual build or unknown CI
  set (OPENKIT_BUILD_VERSION 0)
endif ()
set (OPENKIT_VERSION_STRING "${OPENKIT_MAJOR_VERSION}.${OPENKIT_MINOR_VERSION}.${OPENKIT_BUGFIX_VERSION}.${OPENKIT_BUILD_VERSION}")

# Store the git hash of the current head
if(EXISTS "${CMAKE_SOURCE_DIR}/.git/HEAD")

  file(READ "${CMAKE_SOURCE_DIR}/.git/HEAD" OPEN_KIT_SOURCE_VERSION)

  if("${OPEN_KIT_SOURCE_VERSION}" MATCHES "^ref:")
    string(REGEX REPLACE "^ref: *([^ \n\r]*).*" "\\1" OPEN_KIT_GIT_REF "${OPEN_KIT_SOURCE_VERSION}")
    file(READ "${PROJECT_SOURCE_DIR}/.git/${OPEN_KIT_GIT_REF}" OPEN_KIT_SOURCE_VERSION)
  endif()

  string(STRIP "${OPEN_KIT_SOURCE_VERSION}" OPEN_KIT_SOURCE_VERSION)
endif ()

# store the build date time (note, it's stored in UTC)
string (TIMESTAMP OPEN_KIT_BUILD_DATE_TIME "%Y-%m-%dT%H:%M:%SZ" UTC)
string (REGEX REPLACE "^(.+)T(.+)Z$" "\\1" OPEN_KIT_BUILD_DATE "${OPEN_KIT_BUILD_DATE_TIME}")
string (REGEX REPLACE "^(.+)T(.+)Z$" "\\2" OPEN_KIT_BUILD_TIME "${OPEN_KIT_BUILD_DATE_TIME}")
string (REGEX REPLACE "^([0-9]+)-([0-9]+)-([0-9]+)$" "\\1" OPEN_KIT_BUILD_YEAR "${OPEN_KIT_BUILD_DATE}")

# store some general information
set(OPENKIT_TITLE "OpenKit Native")
set(OPENKIT_DESCRIPTION "Dynatrace OpenKit SDK for C/C++")
set(OPENKIT_COMPANY_NAME "Dynatrace LLC")
if ("${OPEN_KIT_BUILD_YEAR}" STREQUAL "2018")
    set(OPENKIT_COPYRIGHT "(c) 2018 ${OPENKIT_COMPANY_NAME}")
else ()
    set(OPENKIT_COPYRIGHT "(c) 2018-${OPEN_KIT_BUILD_YEAR} ${OPENKIT_COMPANY_NAME}")
endif()
