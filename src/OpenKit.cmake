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

set(OPENKIT_PUBLIC_HEADERS_CXX_API
    ${CMAKE_SOURCE_DIR}/include/OpenKit/AbstractOpenKitBuilder.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/AppMonOpenKitBuilder.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/CrashReportingLevel.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/DataCollectionLevel.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/DynatraceOpenKitBuilder.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/IAction.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/ILogger.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/IOpenKit.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/IRootAction.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/ISession.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/ISSLTrustManager.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/IWebRequestTracer.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/OpenKitConstants.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit.h
)

set(OPENKIT_PUBLIC_HEADERS_C_API
    ${CMAKE_SOURCE_DIR}/include/api-c/OpenKit-c.h
)

set(OPENKIT_SOURCES_CXX_API
    ${CMAKE_CURRENT_LIST_DIR}/api/AbstractOpenKitBuilder.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api/AppMonOpenKitBuilder.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api/DynatraceOpenKitBuilder.cxx
)

set(OPENKIT_SOURCES_C_API
    ${CMAKE_CURRENT_LIST_DIR}/api-c/CustomLogger.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api-c/CustomLogger.h
    ${CMAKE_CURRENT_LIST_DIR}/api-c/CustomTrustManager.h
    ${CMAKE_CURRENT_LIST_DIR}/api-c/CustomTrustManager.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api-c/OpenKit-c.cxx
)

set(OPENKIT_SOURCES_CACHING
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCache.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCache.h
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheEntry.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheEntry.h
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheEvictor.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheEvictor.h
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheRecord.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheRecord.h
    ${CMAKE_CURRENT_LIST_DIR}/caching/IBeaconCache.h
    ${CMAKE_CURRENT_LIST_DIR}/caching/IObserver.h
    ${CMAKE_CURRENT_LIST_DIR}/caching/SpaceEvictionStrategy.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/SpaceEvictionStrategy.h
    ${CMAKE_CURRENT_LIST_DIR}/caching/TimeEvictionStrategy.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/TimeEvictionStrategy.h
)

set(OPENKIT_SOURCES_COMMUNICATION
    ${CMAKE_CURRENT_LIST_DIR}/communication/AbstractBeaconSendingState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/AbstractBeaconSendingState.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingCaptureOffState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingCaptureOffState.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingCaptureOnState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingCaptureOnState.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingContext.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingContext.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingFlushSessionsState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingFlushSessionsState.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingInitialState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingInitialState.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingRequestUtil.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingRequestUtil.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingResponseUtil.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingResponseUtil.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingTerminalState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingTerminalState.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingTimeSyncState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingTimeSyncState.h
)

set(OPENKIT_SOURCES_CONFIGURATION
    ${CMAKE_CURRENT_LIST_DIR}/configuration/BeaconCacheConfiguration.cxx
    ${CMAKE_CURRENT_LIST_DIR}/configuration/BeaconCacheConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/configuration/BeaconConfiguration.cxx
    ${CMAKE_CURRENT_LIST_DIR}/configuration/BeaconConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/configuration/Configuration.cxx
    ${CMAKE_CURRENT_LIST_DIR}/configuration/Configuration.h
    ${CMAKE_CURRENT_LIST_DIR}/configuration/Device.cxx
    ${CMAKE_CURRENT_LIST_DIR}/configuration/Device.h
    ${CMAKE_CURRENT_LIST_DIR}/configuration/HTTPClientConfiguration.cxx
    ${CMAKE_CURRENT_LIST_DIR}/configuration/HTTPClientConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/configuration/OpenKitType.cxx
    ${CMAKE_CURRENT_LIST_DIR}/configuration/OpenKitType.h
)

set(OPENKIT_SOURCES_CORE_UTIL
    ${CMAKE_CURRENT_LIST_DIR}/core/util/Compressor.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/Compressor.h
    ${CMAKE_CURRENT_LIST_DIR}/core/util/CountDownLatch.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/CountDownLatch.h
    ${CMAKE_CURRENT_LIST_DIR}/core/util/CyclicBarrier.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/CyclicBarrier.h
    ${CMAKE_CURRENT_LIST_DIR}/core/util/DefaultLogger.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/DefaultLogger.h
    ${CMAKE_CURRENT_LIST_DIR}/core/util/InetAddressValidator.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/InetAddressValidator.h
    ${CMAKE_CURRENT_LIST_DIR}/core/util/ReadWriteLock.h
    ${CMAKE_CURRENT_LIST_DIR}/core/util/ScopedReadLock.h
    ${CMAKE_CURRENT_LIST_DIR}/core/util/ScopedWriteLock.h
    ${CMAKE_CURRENT_LIST_DIR}/core/util/SynchronizedQueue.h
    ${CMAKE_CURRENT_LIST_DIR}/core/util/URLEncoding.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/URLEncoding.h
)

set(OPENKIT_SOURCES_CORE
    ${CMAKE_CURRENT_LIST_DIR}/core/Action.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/Action.h
    ${CMAKE_CURRENT_LIST_DIR}/core/ActionCommonImpl.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/ActionCommonImpl.h
    ${CMAKE_CURRENT_LIST_DIR}/core/BeaconSender.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/BeaconSender.h
    ${CMAKE_CURRENT_LIST_DIR}/core/NullAction.h
    ${CMAKE_CURRENT_LIST_DIR}/core/NullRootAction.h
    ${CMAKE_CURRENT_LIST_DIR}/core/NullSession.h
    ${CMAKE_CURRENT_LIST_DIR}/core/NullWebRequestTracer.h
    ${CMAKE_CURRENT_LIST_DIR}/core/OpenKit.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/OpenKit.h
    ${CMAKE_CURRENT_LIST_DIR}/core/RootAction.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/RootAction.h
    ${CMAKE_CURRENT_LIST_DIR}/core/Session.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/Session.h
    ${CMAKE_CURRENT_LIST_DIR}/core/SessionWrapper.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/SessionWrapper.h
    ${CMAKE_CURRENT_LIST_DIR}/core/UTF8String.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/UTF8String.h
    ${CMAKE_CURRENT_LIST_DIR}/core/WebRequestTracerBase.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/WebRequestTracerBase.h
    ${CMAKE_CURRENT_LIST_DIR}/core/WebRequestTracerStringURL.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/WebRequestTracerStringURL.h
)

set(OPENKIT_SOURCES_PROTOCOL_SSL
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ssl/SSLBlindTrustManager.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ssl/SSLBlindTrustManager.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ssl/SSLStrictTrustManager.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ssl/SSLStrictTrustManager.cxx
)

set(OPENKIT_SOURCES_PROTOCOL
    ${CMAKE_CURRENT_LIST_DIR}/protocol/Beacon.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/Beacon.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/BeaconProtocolConstants.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/EventType.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/HTTPClient.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/HTTPClient.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/HTTPResponseParser.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/HTTPResponseParser.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/IHTTPClient.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/Response.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/Response.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/StatusResponse.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/StatusResponse.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/TimeSyncResponse.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/TimeSyncResponse.h
)

set(OPENKIT_SOURCES_PROVIDERS
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultHTTPClientProvider.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultHTTPClientProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultPRNGenerator.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultPRNGenerator.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultSessionIDProvider.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultSessionIDProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultThreadIDProvider.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultThreadIDProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultTimingProvider.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultTimingProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/IHTTPClientProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/IPRNGenerator.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/ISessionIDProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/IThreadIDProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/ITimingProvider.h
)

# Create a combined list of all source files
set(OPENKIT_SOURCES
    ${OPENKIT_PUBLIC_HEADERS_C_API}
    ${OPENKIT_SOURCES_C_API}
    ${OPENKIT_PUBLIC_HEADERS_CXX_API}
    ${OPENKIT_SOURCES_CXX_API}
    ${OPENKIT_SOURCES_CACHING}
    ${OPENKIT_SOURCES_CORE}
    ${OPENKIT_SOURCES_CORE_UTIL}
    ${OPENKIT_SOURCES_COMMUNICATION}
    ${OPENKIT_SOURCES_PROVIDERS}
    ${OPENKIT_SOURCES_PROTOCOL_SSL}
    ${OPENKIT_SOURCES_PROTOCOL}
    ${OPENKIT_SOURCES_CONFIGURATION}
)

include(CompilerConfiguration)
fix_compiler_flags()

##
# Utility macro for generating OpenKit version header file
macro(_generate_open_kit_version_header)

    set(OPENKIT_BUILD_YEAR "${OPEN_KIT_BUILD_YEAR}")
    set(OPENKIT_VERSION_MAJOR ${OPENKIT_MAJOR_VERSION})
    set(OPENKIT_VERSION_MINOR ${OPENKIT_MINOR_VERSION})
    set(OPENKIT_VERSION_BUGFIX ${OPENKIT_BUGFIX_VERSION})
    set(OPENKIT_BUILD_NUMBER ${OPENKIT_BUILD_VERSION})

    set(VERSION_HEADER_TEMPLATE "${CMAKE_SOURCE_DIR}/cmake/Templates/OpenKitVersion.h.in")
    set(VERSION_HEADER_OUTPUT "${CMAKE_BINARY_DIR}/include/OpenKitVersion.h")
    configure_file(${VERSION_HEADER_TEMPLATE} ${VERSION_HEADER_OUTPUT} @ONLY)
endmacro()

##
# Utility macro for generating OpenKit version.rc (ressource) file
macro(_generate_open_kit_version_rc)

    if (NOT BUILD_SHARED_LIBS OR NOT MSVC)
        message(FATAL_ERROR "Only invoke when building OpenKit as dll (MSVC specific)")
    endif ()

    # setup RC internal values
    set(FILE_TYPE "VFT_DLL")
    if (NOT ("$ENV{TRAVIS}" STREQUAL "")           # Travis CI 
        OR NOT ("$ENV{APPVEYOR}" STREQUAL ""))     # AppVeyor
        set(ADDITIONAL_FILE_FLAGS "0L")
    else ()
        set(ADDITIONAL_FILE_FLAGS "VS_FF_PRERELEASE|VS_FF_PRIVATEBUILD")
    endif ()
        
    # setup version, Copyright and such ...
    set (PRODUCT_VERSION "${OPENKIT_MAJOR_VERSION},${OPENKIT_MINOR_VERSION},${OPENKIT_BUGFIX_VERSION},${OPENKIT_BUILD_VERSION}")
    set (PRODUCT_VERSION_STR "${OPENKIT_VERSION_STRING}")
    set (FILE_VERSION "${OPENKIT_MAJOR_VERSION},${OPENKIT_MINOR_VERSION},${OPENKIT_BUGFIX_VERSION},${OPENKIT_BUILD_VERSION}")
    set (FILE_VERSION_STR "${OPENKIT_VERSION_STRING}")
    set (COMPANY_NAME "${OPENKIT_COMPANY_NAME}")
    set (COPYRIGHT_STR "${OPENKIT_COPYRIGHT}")
    set (PRODUCT_NAME_STR "${OPENKIT_TITLE}")
    set (INTERNAL_NAME_STR "${OPENKIT_TITLE}")
    set (FILE_DESCRIPTION "${OPENKIT_DESCRIPTION}")

    set(VERSION_RC_TEMPLATE "${CMAKE_SOURCE_DIR}/cmake/Templates/version.rc.in")
    set(VERSION_RC_OUTPUT "${CMAKE_BINARY_DIR}/rc/version.rc")
    configure_file(${VERSION_RC_TEMPLATE} ${VERSION_RC_OUTPUT} @ONLY)
endmacro()

##
# Function to build the OpenKit target
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
    
    # generate OpenKitVersion.h
    _generate_open_kit_version_header()
    set (OPENKIT_LIB_SOURCES ${OPENKIT_SOURCES})
    if (BUILD_SHARED_LIBS AND MSVC)
        # generate version.rc in case of shared lib build
        _generate_open_kit_version_rc()
        set (OPENKIT_LIB_SOURCES ${OPENKIT_LIB_SOURCES} ${VERSION_RC_OUTPUT})
    endif()

    open_kit_build_library(OpenKit "${OPENKIT_INCLUDE_DIRS}" "${OPENKIT_LIBS}" ${OPENKIT_LIB_SOURCES})

    # enforce usage of C++11 for OpenKit
    enforce_cxx11_standard(OpenKit)

    # add special processor flag when building OpenKit as static library
    if(NOT BUILD_SHARED_LIBS)
        # For a static library we set the compiler flag OPENKIT_STATIC_DEFINE
        target_compile_definitions(OpenKit PRIVATE -DOPENKIT_STATIC_DEFINE)
    endif()

    # add version & soversion target properties
    if (BUILD_SHARED_LIBS AND NOT MSVC)
        set_target_properties(OpenKit PROPERTIES
                              VERSION "${OPENKIT_MAJOR_VERSION}.${OPENKIT_MINOR_VERSION}.${OPENKIT_BUGFIX_VERSION}.${OPENKIT_BUILD_VERSION}"
                              SOVERSION "${OPENKIT_MAJOR_VERSION}")
    endif ()

    # add special preprocessor flag when curl is used as static library
    if (NOT BUILD_SHARED_LIBS OR OPENKIT_MONOLITHIC_SHARED_LIB)
        target_compile_definitions(OpenKit PRIVATE -DCURL_STATICLIB)
    endif()

    # generate export header
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
        ${CMAKE_BINARY_DIR}/include/OpenKitVersion.h
        DESTINATION "${INSTALL_INC_DIR}"
    )

    set_target_properties(OpenKit PROPERTIES FOLDER Lib)
    source_group("Header Files\\API - C" FILES ${OPENKIT_PUBLIC_HEADERS_C_API})
    source_group("Source Files\\API - C" FILES ${OPENKIT_SOURCES_C_API})
    source_group("Header Files\\API - C++" FILES ${OPENKIT_PUBLIC_HEADERS_CXX_API})
    source_group("Source Files\\API - C++" FILES ${OPENKIT_SOURCES_CXX_API})
    source_group("Source Files\\Caching" FILES ${OPENKIT_SOURCES_CACHING})
    source_group("Source Files\\Core" FILES ${OPENKIT_SOURCES_CORE})
    source_group("Source Files\\Core\\Util" FILES ${OPENKIT_SOURCES_CORE_UTIL})
    source_group("Source Files\\Communication" FILES ${OPENKIT_SOURCES_COMMUNICATION})
    source_group("Source Files\\Protocol" FILES ${OPENKIT_SOURCES_PROTOCOL})
    source_group("Source Files\\Protocol\\SSL" FILES ${OPENKIT_SOURCES_PROTOCOL_SSL})
    source_group("Source Files\\Providers" FILES ${OPENKIT_SOURCES_PROVIDERS})
    source_group("Source Files\\Configuration" FILES ${OPENKIT_SOURCES_CONFIGURATION})

    if (BUILD_SHARED_LIBS AND MSVC)
        # add generated version.rc to Resource Files source group
        source_group("Resource Files" FILES ${VERSION_RC_OUTPUT})
    endif ()

endfunction()
