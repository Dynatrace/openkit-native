remove_strict_warnings_for_testcode()

include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})

################
# CURL showcase
################
set(SOURCES_CURLTEST
	communication/curltest.cxx
)

# Adds the executable "curltest"
add_executable( curltest
	# Sources required by the curltest showcase
	${CMAKE_SOURCE_DIR}/src/core/util/Compressor.cxx

	# Test files
    ${SOURCES_CURLTEST}
)

# Link the CURL and ZLIB libraries
target_link_libraries(curltest ${CURLIMPLIB} ${ZLIB_LIBRARIES})

# Setting the compiler flag OPENKIT_STATIC_DEFINE leaves any export definitions empty
target_compile_definitions(curltest PRIVATE -DOPENKIT_STATIC_DEFINE)

################
# Unit tests
################

set(SOURCES_TO_TEST)
if(BUILD_SHARED_LIBS)
	# The unit tests don't test the public OpenKit-API, but instead they test the OpenKit internal functions.
	# If the OpenKit is built as a shared library, we don't want to export all (internal) symbols.
	# Therefore, for the target "unittests", the sources are directly accessed and compiled.
	# This is done by making the list of sources available here and creating the releative path with prepend.
	prepend_to_list(SOURCES_TO_TEST ../src/ ${SOURCES})
endif()
 
set(SOURCES_UNITTEST
	# Sources to be tested
	${SOURCES_TO_TEST} 

	# Test files
	core/UTF8StringTest.cxx
	core/util/CompressorTest.cxx
	core/util/URLEncodingTest.cxx
	core/util/SynchronizedQueueTest.cxx
	core/util/InetAddressValidatorTest.cxx
	protocol/StatusResponseTest.cxx
	protocol/TimeSyncResponseTest.cxx
	providers/DefaultTimingProviderTest.cxx
	providers/DefaultSessionIDProviderTest.cxx
	providers/DefaultThreadIDProviderTest.cxx
	communication/AbstractBeaconSendingStateTest.cxx
	communication/BeaconSendingTerminalStateTest.cxx
	communication/BeaconSendingInitialStateTest.cxx
	communication/BeaconSendingTimeSyncStateTest.cxx
	communication/BeaconSendingCaptureOffStateTest.cxx
	communication/BeaconSendingCaptureOnStateTest.cxx
	communication/BeaconSendingContextTest.cxx
	configuration/ConfigurationTest.cxx
	configuration/BeaconCacheConfigurationTest.cxx
	caching/BeaconCacheEntryTest.cxx
	caching/BeaconCacheRecordTest.cxx
	caching/SpaceEvictionStrategyTest.cxx
	caching/TimeEvictionStrategyTest.cxx
	caching/BeaconCacheEvictorTest.cxx
	caching/BeaconCacheTest.cxx
)

# Adds the executable "unittest"
add_executable( unittests
    ${SOURCES_UNITTEST}
)

# Link the CURL and ZLIB libraries, as well as the libraries for Google testing.
if(BUILD_SHARED_LIBS)
	target_link_libraries(unittests ${CURLIMPLIB} ${ZLIB_LIBRARIES} ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} ${GMOCK_MAIN_LIBRARIES})
else()
	target_link_libraries(unittests OpenKit ${CURLIMPLIB} ${ZLIB_LIBRARIES} ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} ${GMOCK_MAIN_LIBRARIES})
endif()
target_compile_definitions(unittests PRIVATE -DOPENKIT_STATIC_DEFINE)

if(NOT WIN32)
	target_link_libraries(unittests pthread)	# TODO Johannes Baeuerle. Check if pthread is necessary
else()
	add_custom_command(TARGET curltest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlib.dll ${CMAKE_BINARY_DIR}/bin
	)
	add_custom_command(TARGET unittests POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlib.dll ${CMAKE_BINARY_DIR}/bin
	)
endif()

ADD_TEST(NAME CurlTest COMMAND curltest WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_TEST(NAME UnitTests COMMAND unittests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
