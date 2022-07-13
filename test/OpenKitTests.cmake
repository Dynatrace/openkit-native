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
# limitations under the License.macro(build_open_kit_tests)


if (NOT OPENKIT_BUILD_TESTS)
    message(INFO "OPENKIT_BUILD_TESTS is disabled - skip building OpenKit tests...")
    return()
endif ()

set(OPENKIT_SOURCES_TEST
    ${CMAKE_CURRENT_LIST_DIR}/DefaultValues.cxx
    ${CMAKE_CURRENT_LIST_DIR}/DefaultValues.h
)

set(OPENKIT_SOURCES_TEST_API
    ${CMAKE_CURRENT_LIST_DIR}/api/AbstractOpenKitBuilderTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api/AppMonOpenKitBuilderTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api/DynatraceOpenKitBuilderTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api/LogLevelTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/api/mock/MockIHttpRequest.h
    ${CMAKE_CURRENT_LIST_DIR}/api/mock/MockIHttpRequestInterceptor.h
    ${CMAKE_CURRENT_LIST_DIR}/api/mock/MockIHttpResponse.h
    ${CMAKE_CURRENT_LIST_DIR}/api/mock/MockIHttpResponseInterceptor.h
    ${CMAKE_CURRENT_LIST_DIR}/api/mock/MockILogger.h
    ${CMAKE_CURRENT_LIST_DIR}/api/mock/MockIOpenKitBuilder.h
    ${CMAKE_CURRENT_LIST_DIR}/api/mock/MockIRootAction.h
    ${CMAKE_CURRENT_LIST_DIR}/api/mock/MockISslTrustManager.h
    ${CMAKE_CURRENT_LIST_DIR}/api/mock/MockIWebRequestTracer.h
)

set(OPENKIT_SOURCES_TEST_CORE
    ${CMAKE_CURRENT_LIST_DIR}/core/SessionWatchdogContextTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/SessionWatchdogTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/UTF8StringTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/mock/MockIBeaconSender.h
    ${CMAKE_CURRENT_LIST_DIR}/core/mock/MockISessionWatchdog.h
    ${CMAKE_CURRENT_LIST_DIR}/core/mock/MockISessionWatchdogContext.h
)

set(OPENKIT_SOURCES_TEST_CORE_OBJECTS
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/ActionCommonImplTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/LeafActionTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullActionTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullRootActionTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullSessionTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/NullWebRequestTracerTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/OpenKitInitializerTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/OpenKitTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/RootActionTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/SessionCreatorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/SessionProxyTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/SessionTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/WebRequestTracerTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/WebRequestTracerURLValidityTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/builder/TestOpenKitBuilder.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/builder/TestSessionBuilder.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/mock/MockIActionCommon.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/mock/MockICancelableOpenKitObject.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/mock/MockIOpenKitComposite.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/mock/MockIOpenKitInitializer.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/mock/MockIOpenKitObject.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/mock/MockISessionCreator.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/mock/MockISessionCreatorInput.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/mock/MockISessionProxy.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/mock/MockIWebRequestTracerInternals.h
    ${CMAKE_CURRENT_LIST_DIR}/core/objects/mock/MockSessionInternals.h
)

set(OPENKIT_SOURCES_TEST_CORE_UTIL
    ${CMAKE_CURRENT_LIST_DIR}/core/util/CompressorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/DefaultLoggerTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/InetAddressValidatorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/InterruptibleThreadSuspenderTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/StringUtilTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/SynchronizedQueueTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/ThreadSurrogateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util/URLEncodingTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/util//mock/MockIInterruptibleThreadSuspender.h
)

set(OPENKIT_SOURCES_TEST_PROTOCOL
    ${CMAKE_CURRENT_LIST_DIR}/protocol/StatusResponseTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/HTTPResponseParserTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/BeaconTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/JsonResponseParserTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/KeyValueResponseParserTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseAttributesDefaultsTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseAttributesTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/ResponseParserTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/builder/TestBeaconBuilder.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/mock/MockIAdditionalQueryParameters.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/mock/MockIBeacon.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/mock/MockIBeaconInitializer.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/mock/MockIHTTPClient.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/mock/MockIResponseAttributes.h
    ${CMAKE_CURRENT_LIST_DIR}/protocol/mock/MockIStatusResponse.h
)

set(OPENKIT_SOURCES_TEST_PROTOCOL_HTTP
    ${CMAKE_CURRENT_LIST_DIR}/protocol/http/HttpHeaderCollectionTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/http/HttpHeaderUtilTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/http/HttpRequestTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/http/HttpResponseTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/http/NullHttpRequestInterceptorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/protocol/http/NullHttpResponseInterceptorTest.cxx
)

set(OPENKIT_SOURCES_TEST_PROVIDERS
    ${CMAKE_CURRENT_LIST_DIR}/providers/mock/MockIHTTPClientProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/mock/MockIPRNGenerator.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/mock/MockISessionIDProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/mock/MockIThreadIDProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/mock/MockITimingProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultPRNGeneratorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultSessionIDProviderTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultThreadIDProviderTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/DefaultTimingProviderTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/FixedPRNGeneratorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/providers/FixedSessionIDProviderTest.cxx
)

set(OPENKIT_SOURCES_TEST_CORE_COMMUNICATION
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/AbstractBeaconSendingStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingCaptureOffStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingCaptureOnStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingContextTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingFlushSessionStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingInitialStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingRequestUtilTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingResponseUtilTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/BeaconSendingTerminalStateTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/CustomMatchers.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/builder/TestBeaconSendingContextBuilder.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/mock/MockAbstractBeaconSendingState.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/mock/MockIBeaconSendingContext.h
    ${CMAKE_CURRENT_LIST_DIR}/core/communication/mock/MockIBeaconSendingState.h
)

set(OPENKIT_SOURCES_TEST_CORE_CONFIGURATION
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/BeaconConfigurationTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/BeaconCacheConfigurationTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/HTTPClientConfigurationTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/OpenKitConfigurationTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/PrivacyConfigurationTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/ServerConfigurationTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/mock/MockIBeaconCacheConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/mock/MockIBeaconConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/mock/MockIHTTPClientConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/mock/MockIOpenKitConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/mock/MockIPrivacyConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configuration/mock/MockIServerConfiguration.h
)

set(OPENKIT_SOURCES_TEST_CORE_CACHING
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCacheEntryTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCacheRecordTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCacheEvictorTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconCacheTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/BeaconKeyTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/SpaceEvictionStrategyTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/TimeEvictionStrategyTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/mock/MockIBeaconCache.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/mock/MockIBeaconCacheEvictionStrategy.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/mock/MockIBeaconCacheEvictor.h
    ${CMAKE_CURRENT_LIST_DIR}/core/caching/mock/MockIObserver.h
)

set(OPENKIT_SOURCES_TEST_UTIL_JSON_LEXER
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonTokenTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/JsonLexerTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/lexer/MockJsonLexer.h
)

set(OPENKIT_SOURCES_TEST_UTIL_JSON_OBJECTS
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonArrayValueTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonBooleanValueTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonNullValueTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonNumberValueTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonObjectValueTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/objects/JsonStringValueTest.cxx
)

set(OPENKIT_SOURCES_TEST_UTIL_JSON_READER
    ${CMAKE_CURRENT_LIST_DIR}/util/json/reader/MockResettableReader.h
    ${CMAKE_CURRENT_LIST_DIR}/util/json/reader/DefaultResettableReaderTest.cxx
)

set(OPENKIT_SOURCES_TEST_UTIL_JSON
    ${CMAKE_CURRENT_LIST_DIR}/util/json/JsonParserTest.cxx
    ${CMAKE_CURRENT_LIST_DIR}/util/json/JsonWriterTest.cxx
)

set(OPENKIT_SOURCES_UNITTEST
    # Test files
    ${OPENKIT_SOURCES_TEST}
    ${OPENKIT_SOURCES_TEST_API}
    ${OPENKIT_SOURCES_TEST_CORE}
    ${OPENKIT_SOURCES_TEST_CORE_CACHING}
    ${OPENKIT_SOURCES_TEST_CORE_COMMUNICATION}
    ${OPENKIT_SOURCES_TEST_CORE_CONFIGURATION}
    ${OPENKIT_SOURCES_TEST_CORE_OBJECTS}
    ${OPENKIT_SOURCES_TEST_CORE_UTIL}
    ${OPENKIT_SOURCES_TEST_PROTOCOL}
    ${OPENKIT_SOURCES_TEST_PROTOCOL_HTTP}
    ${OPENKIT_SOURCES_TEST_PROVIDERS}
    ${OPENKIT_SOURCES_TEST_UTIL_JSON_LEXER}
    ${OPENKIT_SOURCES_TEST_UTIL_JSON_OBJECTS}
    ${OPENKIT_SOURCES_TEST_UTIL_JSON_READER}
    ${OPENKIT_SOURCES_TEST_UTIL_JSON}
)

include(CompilerConfiguration)
fix_compiler_flags()

function(build_open_kit_tests)

    set (TEST_NAME OpenKitTest)

    message("Configuring ${TEST_NAME} ... ")

    include(CompilerConfiguration)
    include(BuildFunctions)
    
    # add OpenKitTest target
    open_kit_add_test(${TEST_NAME} ${OPENKIT_SOURCES_UNITTEST})

    SET(OPENKIT_LIB OpenKit)
    if (BUILD_SHARED_LIBS)
        # The unit tests don't test the public OpenKit-API, but instead they test the OpenKit internals.
        # Internal functions and methods are not exported from the OpenKit shared lib, therefore
        # the OpenKit library is compiled as static lib again and linked to our test
        SET(OPENKIT_LIB OpenKit_UnderTest)
        build_open_kit(BUILD_FOR_TEST TEST_LIB_NAME ${OPENKIT_LIB})
    endif()

    # tests require the private headers from OpenKit
    # also add the src directory
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    
    target_link_libraries(${TEST_NAME} PRIVATE Dynatrace::${OPENKIT_LIB})
    
    if (WIN32 AND BUILD_SHARED_LIBS AND NOT OPENKIT_MONOLITHIC_SHARED_LIB)
       add_custom_command ( TARGET ${TEST_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:zlib> $<TARGET_FILE_DIR:OpenKitTest>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libcurl> $<TARGET_FILE_DIR:OpenKitTest>  )
    endif()

    set_target_properties(${TEST_NAME} PROPERTIES FOLDER Tests)

    source_group("Source Files" FILES ${OPENKIT_SOURCES_TEST})
    source_group("Source Files\\API" FILES ${OPENKIT_SOURCES_TEST_API})
    source_group("Source Files\\Core" FILES ${OPENKIT_SOURCES_TEST_CORE})
    source_group("Source Files\\Core\\Caching" FILES ${OPENKIT_SOURCES_TEST_CORE_CACHING})
    source_group("Source Files\\Core\\Communication" FILES ${OPENKIT_SOURCES_TEST_CORE_COMMUNICATION})
    source_group("Source Files\\Core\\Configuration" FILES ${OPENKIT_SOURCES_TEST_CORE_CONFIGURATION})
    source_group("Source Files\\Core\\Objects" FILES ${OPENKIT_SOURCES_TEST_CORE_OBJECTS})
    source_group("Source Files\\Core\\Util" FILES ${OPENKIT_SOURCES_TEST_CORE_UTIL})
    source_group("Source Files\\JSON" FILES ${OPENKIT_SOURCES_TEST_UTIL_JSON})
    source_group("Source Files\\JSON\\Lexer" FILES ${OPENKIT_SOURCES_TEST_UTIL_JSON_LEXER})
    source_group("Source Files\\JSON\\Objects" FILES ${OPENKIT_SOURCES_TEST_UTIL_JSON_OBJECTS})
    source_group("Source Files\\JSON\\Reader" FILES ${OPENKIT_SOURCES_TEST_UTIL_JSON_READER})
    source_group("Source Files\\Protocol" FILES ${OPENKIT_SOURCES_TEST_PROTOCOL})
    source_group("Source Files\\Providers" FILES ${OPENKIT_SOURCES_TEST_PROVIDERS})

endfunction()
