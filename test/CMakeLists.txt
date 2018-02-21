remove_strict_warnings_for_testcode()

include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})

set(SOURCES_CURLTEST
	communication/curltest.cxx
)

add_executable( curltest 
    ${SOURCES_CURLTEST}
)

target_link_libraries(curltest LibBase ${CURLIMPLIB} ${ZLIB_LIBRARIES})


set(SOURCES_UNITTEST
	core/UTF8StringTest.cxx
	core/util/CompressorTest.cxx
	providers/DefaultTimingProviderTest.cxx
	communication/AbstractBeaconSendingStateTest.cxx
	communication/BeaconSendingTerminalStateTest.cxx
	communication/BeaconSendingInitialStateTest.cxx
)

add_executable( unittests 
    ${SOURCES_UNITTEST}
)


target_link_libraries(unittests LibBase ${CURLIMPLIB} ${ZLIB_LIBRARIES} ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} ${GMOCK_MAIN_LIBRARIES})

if(NOT WIN32)
	target_link_libraries(unittests pthread)
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
