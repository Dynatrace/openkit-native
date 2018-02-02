include_directories(${GTEST_INCLUDE_DIRS})

set(SOURCES_CURLTEST
	src/curltest.cxx
)



add_executable( curltest 
    ${SOURCES_CURLTEST}
)

if(WIN32)
	
	add_custom_command(TARGET curltest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlibd.dll ${CMAKE_BINARY_DIR}/bin
	)
endif()

target_link_libraries(curltest LibBase ${CURLIMPLIB} ${ZLIB_LIBRARIES})

ADD_TEST(NAME CurlTest COMMAND curltest WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
