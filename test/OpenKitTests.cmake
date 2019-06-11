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
# limitations under the License.macro(build_open_kit_tests)


if (NOT OPENKIT_BUILD_TESTS)
    message(INFO "OPENKIT_BUILD_TESTS is disabled - skip building OpenKit tests...")
    return()
endif ()

set(SOURCES_TO_TEST)
if(BUILD_SHARED_LIBS)
    # The unit tests don't test the public OpenKit-API, but instead they test the OpenKit internal functions.
    # If the OpenKit is built as a shared library, we don't want to export all (internal) symbols.
    # Therefore, for the target "unittests", the sources are directly accessed and compiled.
    # This is done by making the list of sources available here and creating the releative path with prepend.
    set(SOURCES_TO_TEST ${OPENKIT_SOURCES})
endif()

set(OPENKIT_SOURCES_TEST_API
    ${CMAKE_CURRENT_LIST_DIR}/api/LogLevelTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api/OpenKitBuilderTest.cxx
)

set(OPENKIT_SOURCES_TEST_CORE
    ${CMAKE_CURRENT_LIST_DIR}/core/UTF8StringTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/SessionTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/ActionTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/RootActionTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/WebRequestTracerTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/WebRequestTracerURLValidityTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/CompressorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/URLEncodingTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/SynchronizedQueueTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/InetAddressValidatorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/MockBeaconSender.h
    ${CMAKE_CURRENT_LIST_DIR}/core/MockSession.h
    ${CMAKE_CURRENT_LIST_DIR}/core/util/DefaultLoggerTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/MockWebRequestTracer.h
    ${CMAKE_CURRENT_LIST_DIR}/core/MockAction.h
    ${CMAKE_CURRENT_LIST_DIR}/core/MockRootAction.h
)

set(OPENKIT_SOURCES_TEST_PROTOCOL
    ${CMAKE_CURRENT_LIST_DIR}/protocol/StatusResponseTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/MockBeacon.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/MockHTTPClient.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/TestSSLTrustManager.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/HTTPResponseParserTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/BeaconTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/MockStatusResponse.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/NullLogger.h
)

set(OPENKIT_SOURCES_TEST_PROVIDERS
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultSessionIDProviderTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/MockSessionIDProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultThreadIDProviderTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/MockHTTPClientProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/MockPRNGenerator.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultPRNGeneratorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/MockTimingProvider.h
)

set(OPENKIT_SOURCES_TEST_COMMUNICATION
    ${CMAKE_CURRENT_LIST_DIR}/communication/AbstractBeaconSendingStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingCaptureOffStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingCaptureOnStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingContextTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingFlushSessionStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingInitialStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingRequestUtilTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingResponseUtilTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/BeaconSendingTerminalStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/communication/CustomMatchers.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/MockAbstractBeaconSendingState.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/MockBeaconSendingContext.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/TestBeaconSendingState.h
)

set(OPENKIT_SOURCES_TEST_CONFIGURATION
    ${CMAKE_CURRENT_LIST_DIR}/configuration/ConfigurationTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/configuration/BeaconConfigurationTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/configuration/BeaconCacheConfigurationTest.cxx
)

set(OPENKIT_SOURCES_TEST_CACHING
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheEntryTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheRecordTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/SpaceEvictionStrategyTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/TimeEvictionStrategyTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheEvictorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/BeaconCacheTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/caching/MockBeaconCache.h
    ${CMAKE_CURRENT_LIST_DIR}/caching/MockBeaconCacheEvictionStrategy.h
    ${CMAKE_CURRENT_LIST_DIR}/caching/MockObserver.h
)

set(OPENKIT_SOURCES_UNITTEST
    # Test files
    ${OPENKIT_SOURCES_TEST_API}
    ${OPENKIT_SOURCES_TEST_CORE}
    ${OPENKIT_SOURCES_TEST_PROTOCOL}
    ${OPENKIT_SOURCES_TEST_PROVIDERS}
    ${OPENKIT_SOURCES_TEST_COMMUNICATION}
    ${OPENKIT_SOURCES_TEST_CONFIGURATION}
    ${OPENKIT_SOURCES_TEST_CACHING}
)

include(CompilerConfiguration)
fix_compiler_flags()

function(build_open_kit_tests)
    message("Configuring OpenKit  tests... ")

    find_package(ZLIB)
    find_package(CURL)

    set(OPENKIT_TEST_INCLUDE_DIRS
        ${ZLIB_INCLUDE_DIR}
        ${CURL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/include
    )

    set(OPENKIT_TEST_LIBS
        ${ZLIB_LIBRARY}
        ${CURL_LIBRARY}
    )

    include(CompilerConfiguration)
    include(BuildFunctions)

    if (BUILD_SHARED_LIBS)
        ## device under test:
        ## build OpenKit sources as seperate static library when OpenKit itself is build as a shared library
        ## check if CFLAGS or CXXFLAGS are required
        _determine_compiler_language(OpenKit_UnderTest ${SOURCES_TO_TEST})
        open_kit_build_static_library(OpenKit_UnderTest "${OPENKIT_TEST_INCLUDE_DIRS}" "${OPENKIT_TEST_LIBS_LIB_UNDER_TEST}" ${SOURCES_TO_TEST})
        target_compile_definitions(OpenKit_UnderTest PRIVATE -DOPENKIT_STATIC_DEFINE -DCURL_STATICLIB)
        enforce_cxx11_standard(OpenKit_UnderTest)
    endif()

    ## OPENKIT_TEST_LIBS contains the OpenKit_UnderTest library
    open_kit_build_test(OpenKitTest "${OPENKIT_TEST_INCLUDE_DIRS}" "${OPENKIT_TEST_LIBS}" ${OPENKIT_SOURCES_UNITTEST})

    enforce_cxx11_standard(OpenKitTest)
    target_compile_definitions(OpenKitTest PRIVATE -DOPENKIT_STATIC_DEFINE)

    if (NOT BUILD_SHARED_LIBS OR OPENKIT_MONOLITHIC_SHARED_LIB)
        target_compile_definitions(OpenKitTest PRIVATE -DCURL_STATICLIB)
    endif ()
    if (NOT BUILD_SHARED_LIBS)
        target_link_libraries(OpenKitTest PRIVATE OpenKit)
    else()
        target_link_libraries(OpenKitTest PRIVATE OpenKit_UnderTest)
        target_link_libraries(OpenKitTest PRIVATE ${CURL_LIBRARY})
    endif()

    if (WIN32 AND BUILD_SHARED_LIBS AND NOT OPENKIT_MONOLITHIC_SHARED_LIB)
       add_custom_command ( TARGET OpenKitTest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:zlib> $<TARGET_FILE_DIR:OpenKitTest>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libcurl> $<TARGET_FILE_DIR:OpenKitTest>  )
    endif()

    set_target_properties(OpenKitTest PROPERTIES FOLDER Tests)
    if (BUILD_SHARED_LIBS)
        set_target_properties(OpenKit_UnderTest PROPERTIES FOLDER Tests)
    endif()

    source_group("Source Files\\API" FILES ${OPENKIT_SOURCES_TEST_API})
    source_group("Source Files\\Core" FILES ${OPENKIT_SOURCES_TEST_CORE})
    source_group("Source Files\\Protocol" FILES ${OPENKIT_SOURCES_TEST_PROTOCOL})
    source_group("Source Files\\Providers" FILES ${OPENKIT_SOURCES_TEST_PROVIDERS})
    source_group("Source Files\\Communication" FILES ${OPENKIT_SOURCES_TEST_COMMUNICATION})
    source_group("Source Files\\Configuration" FILES ${OPENKIT_SOURCES_TEST_CONFIGURATION})
    source_group("Source Files\\Caching" FILES ${OPENKIT_SOURCES_TEST_CACHING})
    source_group("Source Files\\Caching" FILES ${OPENKIT_SOURCES_TEST_CACHING})

endfunction()
