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

set(OPENKIT_PUBLIC_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/api/OpenKit.h
)

set(OPENKIT_SOURCES_API
	${CMAKE_CURRENT_LIST_DIR}/api/AbstractOpenKitBuilder.cxx
	${CMAKE_CURRENT_LIST_DIR}/api/DynatraceOpenKitBuilder.cxx
	${CMAKE_CURRENT_LIST_DIR}/api/AppMonOpenKitBuilder.cxx
	${CMAKE_CURRENT_LIST_DIR}/api-c/OpenKit-c.cxx
	${CMAKE_CURRENT_LIST_DIR}/api-c/CustomLogger.cxx
	${CMAKE_CURRENT_LIST_DIR}/api-c/CustomTrustManager.cxx
)

set(OPENKIT_SOURCES_CACHING
	${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCache.cxx
	${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheEntry.cxx
	${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheEvictor.cxx
	${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheRecord.cxx
	${CMAKE_CURRENT_LIST_DIR}/caching/SpaceEvictionStrategy.cxx
	${CMAKE_CURRENT_LIST_DIR}/caching/TimeEvictionStrategy.cxx
)

set(OPENKIT_SOURCES_CORE
	${CMAKE_CURRENT_LIST_DIR}/core/util/Compressor.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/util/CountDownLatch.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/util/URLEncoding.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/util/CyclicBarrier.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/util/InetAddressValidator.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/util/DefaultLogger.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/UTF8String.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/BeaconSender.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/Session.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/ActionCommonImpl.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/Action.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/RootAction.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/WebRequestTracerBase.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/WebRequestTracerStringURL.cxx
	${CMAKE_CURRENT_LIST_DIR}/core/OpenKit.cxx
)

set(OPENKIT_SOURCES_COMMUNICATION
	${CMAKE_CURRENT_LIST_DIR}/communication/AbstractBeaconSendingState.cxx
	${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingContext.cxx
	${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingInitialState.cxx
	${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingTerminalState.cxx
	${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingRequestUtil.cxx
	${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingTimeSyncState.cxx
	${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingCaptureOffState.cxx
	${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingCaptureOnState.cxx
	${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingFlushSessionsState.cxx
)

set(OPENKIT_SOURCES_PROTOCOL
	${CMAKE_CURRENT_LIST_DIR}/protocol/Response.cxx
	${CMAKE_CURRENT_LIST_DIR}/protocol/StatusResponse.cxx
	${CMAKE_CURRENT_LIST_DIR}/protocol/TimeSyncResponse.cxx
	${CMAKE_CURRENT_LIST_DIR}/protocol/HTTPClient.cxx
	${CMAKE_CURRENT_LIST_DIR}/protocol/ssl/SSLBlindTrustManager.cxx
	${CMAKE_CURRENT_LIST_DIR}/protocol/ssl/SSLStrictTrustManager.cxx
	${CMAKE_CURRENT_LIST_DIR}/protocol/Beacon.cxx
)

set(OPENKIT_SOURCES_PROVIDERS
	${CMAKE_CURRENT_LIST_DIR}/providers/DefaultTimingProvider.cxx
	${CMAKE_CURRENT_LIST_DIR}/providers/DefaultHTTPClientProvider.cxx
	${CMAKE_CURRENT_LIST_DIR}/providers/DefaultSessionIDProvider.cxx
	${CMAKE_CURRENT_LIST_DIR}/providers/DefaultThreadIDProvider.cxx
)

set(OPENKIT_SOURCES_CONFIGURATION
	${CMAKE_CURRENT_LIST_DIR}/configuration/BeaconCacheConfiguration.cxx
	${CMAKE_CURRENT_LIST_DIR}/configuration/HTTPClientConfiguration.cxx
	${CMAKE_CURRENT_LIST_DIR}/configuration/Configuration.cxx
	${CMAKE_CURRENT_LIST_DIR}/configuration/Device.cxx
	${CMAKE_CURRENT_LIST_DIR}/configuration/OpenKitType.cxx
)

# Create a combined list of all source files
set(OPENKIT_SOURCES
	${OPENKIT_SOURCES_API}
	${OPENKIT_SOURCES_CACHING}
	${OPENKIT_SOURCES_CORE}
	${OPENKIT_SOURCES_COMMUNICATION}
	${OPENKIT_SOURCES_PROVIDERS}
	${OPENKIT_SOURCES_PROTOCOL}
	${OPENKIT_SOURCES_CONFIGURATION}
)

include(CompilerConfiguration)
fix_compiler_flags()

function(build_open_kit)
	message("Configuring OpenKit ... ")

	find_package(ZLIB)
	find_package(CURL)

	set(OPENKIT_INCLUDE_DIRS
		${ZLIB_INCLUDE_DIR}
		${CURL_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_BINARY_DIR}/include
	)

	set(OPENKIT_LIBS
		${ZLIB_LIBRARY}
		${CURL_LIBRARY}
	)

	include(CompilerConfiguration)
	include(BuildFunctions)

	
	open_kit_build_library(OpenKit "${OPENKIT_INCLUDE_DIRS}" "${OPENKIT_LIBS}" ${OPENKIT_SOURCES})

	# enforce usage of C++11 for OpenKit
	enforce_cxx11_standard(OpenKit)

	# add special processor flag when building OpenKit as static library
	if(NOT BUILD_SHARED_LIBS)
		# For a static library we set the compiler flag OPENKIT_STATIC_DEFINE
		target_compile_definitions(OpenKit PRIVATE -DOPENKIT_STATIC_DEFINE)
	endif()

	# add special preprocessor flag when curl is used as static library
	if (NOT BUILD_SHARED_LIBS OR OPENKIT_MONOLITHIC_SHARED_LIB)
		target_compile_definitions(OpenKit PRIVATE -DCURL_STATICLIB)
	endif()

	include(GenerateExportHeader)
	generate_export_header(OpenKit
		BASE_NAME OpenKit
		EXPORT_MACRO_NAME OPENKIT_EXPORT
		EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/OpenKit_export.h
		STATIC_DEFINE OPENKIT_STATIC_DEFINE
	)

	# Add a make target "install". If called (e.g. with "make install") this installs the library files
	install(TARGETS OpenKit
		ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
		LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
		RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
	)
	# This installs the public header files and the export.h file
	install(FILES
		${PUBLIC_HEADERS}
		${CMAKE_BINARY_DIR}/include/OpenKit_export.h
		DESTINATION "${INSTALL_INC_DIR}"
	)

endfunction()
