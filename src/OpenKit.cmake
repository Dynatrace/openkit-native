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

set(OPENKIT_PUBLIC_HEADERS_CXX_API
    ${CMAKE_SOURCE_DIR}/include/OpenKit/AbstractOpenKitBuilder.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/AppMonOpenKitBuilder.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/CrashReportingLevel.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/DataCollectionLevel.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/DynatraceOpenKitBuilder.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/IAction.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/ILogger.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/IOpenKit.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/IOpenKitBuilder.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/IRootAction.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/ISession.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/ISSLTrustManager.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/IWebRequestTracer.h
    ${CMAKE_SOURCE_DIR}/include/OpenKit/LogLevel.h
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
    ${CMAKE_CURRENT_LIST_DIR}/api/LogLevel.cxx
)

set(OPENKIT_SOURCES_C_API
    ${CMAKE_CURRENT_LIST_DIR}/api-c/CustomLogger.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api-c/CustomLogger.h
    ${CMAKE_CURRENT_LIST_DIR}/api-c/CustomTrustManager.h
    ${CMAKE_CURRENT_LIST_DIR}/api-c/CustomTrustManager.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api-c/OpenKit-c.cxx
)

set(OPENKIT_SOURCES_CORE_CACHING
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCache.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCache.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCacheEntry.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCacheEntry.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCacheEvictor.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCacheEvictor.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCacheRecord.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCacheRecord.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/IBeaconCache.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/IBeaconCacheEvictor.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/IObserver.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/SpaceEvictionStrategy.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/SpaceEvictionStrategy.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/TimeEvictionStrategy.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/TimeEvictionStrategy.h
)

set(OPENKIT_SOURCES_CORE_COMMUNICATION
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/AbstractBeaconSendingState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/AbstractBeaconSendingState.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingCaptureOffState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingCaptureOffState.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingCaptureOnState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingCaptureOnState.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingContext.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingContext.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingFlushSessionsState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingFlushSessionsState.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingInitialState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingInitialState.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingRequestUtil.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingRequestUtil.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingResponseUtil.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingResponseUtil.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingTerminalState.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingTerminalState.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/IBeaconSendingContext.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/IBeaconSendingState.h
)

set(OPENKIT_SOURCES_CORE_CONFIGURATION
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/BeaconCacheConfiguration.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/BeaconCacheConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/BeaconConfiguration.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/BeaconConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/ConfigurationDefaults.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/HTTPClientConfiguration.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/HTTPClientConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/IBeaconCacheConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/IBeaconConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/IHTTPClientConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/IOpenKitConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/IPrivacyConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/IServerConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/OpenKitConfiguration.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/OpenKitConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/PrivacyConfiguration.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/PrivacyConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/ServerConfiguration.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/ServerConfiguration.h
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
    ${CMAKE_CURRENT_LIST_DIR}/core/util/StringUtil.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/StringUtil.h
)

set(OPENKIT_SOURCES_CORE
    ${CMAKE_CURRENT_LIST_DIR}/core/BeaconSender.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/BeaconSender.h
    ${CMAKE_CURRENT_LIST_DIR}/core/IBeaconSender.h
    ${CMAKE_CURRENT_LIST_DIR}/core/UTF8String.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/UTF8String.h
)

set(OPENKIT_SOURCES_CORE_OBJECTS
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/LeafAction.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/LeafAction.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/ActionCommonImpl.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/ActionCommonImpl.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/IActionCommon.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/IOpenKitComposite.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/IOpenKitObject.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/IWebRequestTracerInternals.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullAction.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullRootAction.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullRootAction.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullSession.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullSession.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullWebRequestTracer.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullWebRequestTracer.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/OpenKit.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/OpenKit.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/OpenKitComposite.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/OpenKitComposite.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/RootAction.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/RootAction.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/Session.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/Session.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/SessionInternals.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/WebRequestTracer.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/WebRequestTracer.h
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
    ${CMAKE_CURRENT_LIST_DIR}/protocol/IBeacon.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/IHTTPClient.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/IResponseAttributes.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/IStatusResponse.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/JsonResponseParser.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/JsonResponseParser.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/KeyValueResponseParser.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/KeyValueResponseParser.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseAttribute.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseAttributes.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseAttributes.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseAttributesDefaults.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseAttributesDefaults.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseParser.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseParser.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/StatusResponse.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/StatusResponse.h
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

set(OPENKIT_SOURCES_UTIL_JSON_CONSTANTS
    ${CMAKE_CURRENT_LIST_DIR}/util/json/constants/JsonLiterals.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/constants/JsonLiterals.cxx
)

set(OPENKIT_SOURCES_UTIL_JSON_LEXER
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonLexer.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonLexer.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonLexerConstants.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonLexerException.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonLexerException.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonLexerState.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonToken.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonToken.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonTokenType.h
)

set(OPENKIT_SOURCES_UTIL_JSON_READER
    ${CMAKE_CURRENT_LIST_DIR}/util/json/reader/IResettableReader.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/reader/DefaultResettableReader.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/reader/DefaultResettableReader.cxx
)

set(OPENKIT_SOURCES_UTIL_JSON_OBJECTS
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonArrayValue.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonArrayValue.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonBooleanValue.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonBooleanValue.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonNullValue.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonNullValue.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonNumberValue.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonNumberValue.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonObjectValue.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonObjectValue.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonStringValue.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonStringValue.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonValue.h
)

set(OPENKIT_SOURCES_UTIL_JSON_PARSER
    ${CMAKE_CURRENT_LIST_DIR}/util/json/parser/JsonParserState.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/parser/JsonParserException.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/parser/JsonParserException.cxx
)

set(OPENKIT_SOURCES_UTIL_JSON
    ${CMAKE_CURRENT_LIST_DIR}/util/json/JsonParser.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/JsonParser.cxx
)

# Create a combined list of all source files
set(OPENKIT_SOURCES
    ${OPENKIT_PUBLIC_HEADERS_C_API}
    ${OPENKIT_SOURCES_C_API}
    ${OPENKIT_PUBLIC_HEADERS_CXX_API}
    ${OPENKIT_SOURCES_CXX_API}
    ${OPENKIT_SOURCES_CORE}
    ${OPENKIT_SOURCES_CORE_CACHING}
    ${OPENKIT_SOURCES_CORE_COMMUNICATION}
    ${OPENKIT_SOURCES_CORE_CONFIGURATION}
    ${OPENKIT_SOURCES_CORE_OBJECTS}
    ${OPENKIT_SOURCES_CORE_UTIL}
    ${OPENKIT_SOURCES_PROTOCOL_SSL}
    ${OPENKIT_SOURCES_PROTOCOL}
    ${OPENKIT_SOURCES_PROVIDERS}
    ${OPENKIT_SOURCES_UTIL_JSON_CONSTANTS}
    ${OPENKIT_SOURCES_UTIL_JSON_LEXER}
    ${OPENKIT_SOURCES_UTIL_JSON_READER}
    ${OPENKIT_SOURCES_UTIL_JSON_OBJECTS}
    ${OPENKIT_SOURCES_UTIL_JSON_PARSER}
    ${OPENKIT_SOURCES_UTIL_JSON}
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
        target_compile_options(OpenKit PUBLIC -Wno-attributes)
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
    source_group("Source Files\\Core" FILES ${OPENKIT_SOURCES_CORE})
    source_group("Source Files\\Core\\Caching" FILES ${OPENKIT_SOURCES_CORE_CACHING})
    source_group("Source Files\\Core\\Communication" FILES ${OPENKIT_SOURCES_CORE_COMMUNICATION})
    source_group("Source Files\\Core\\Configuration" FILES ${OPENKIT_SOURCES_CORE_CONFIGURATION})
    source_group("Source Files\\Core\\Util" FILES ${OPENKIT_SOURCES_CORE_UTIL})
    source_group("Source Files\\Protocol" FILES ${OPENKIT_SOURCES_PROTOCOL})
    source_group("Source Files\\Protocol\\SSL" FILES ${OPENKIT_SOURCES_PROTOCOL_SSL})
    source_group("Source Files\\Providers" FILES ${OPENKIT_SOURCES_PROVIDERS})

    if (BUILD_SHARED_LIBS AND MSVC)
        # add generated version.rc to Resource Files source group
        source_group("Resource Files" FILES ${VERSION_RC_OUTPUT})
    endif ()

endfunction()
