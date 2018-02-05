include_directories(${GTEST_INCLUDE_DIRS})

set(SOURCES_CURLTEST
	src/curltest.cxx
)

add_executable( curltest 
    ${SOURCES_CURLTEST}
)

target_link_libraries(curltest LibBase ${CURLIMPLIB} ${ZLIB_LIBRARIES})


set(SOURCES_UNITTEST
	src/UTF8StringTest.cxx
)

add_executable( unittests 
    ${SOURCES_UNITTEST}
)

target_link_libraries(unittests LibBase ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
if(NOT WIN32)
	target_link_libraries(unittests pthread)
else()
	add_custom_command(TARGET curltest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlibd.dll ${CMAKE_BINARY_DIR}/bin
	)
endif()


ADD_TEST(NAME CurlTest COMMAND curltest WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_TEST(NAME UnitTests COMMAND unittests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
