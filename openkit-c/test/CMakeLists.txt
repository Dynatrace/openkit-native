include_directories(${GTEST_INCLUDE_DIRS})

set(SOURCES_CURLTEST
	curltest.cxx
)

set(SOURCES_UNITTEST
	utf8stringtest.cxx
)


add_executable( curltest 
    ${SOURCES_CURLTEST}
)

target_link_libraries(curltest LibBase)


add_executable(	unit_tests
	${SOURCES_UNITTEST}
)

target_link_libraries(unit_tests LibBase ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
if(NOT WIN32)
target_link_libraries(unit_tests pthread)
endif()

string(REPLACE "bin/libcurl.dll" "lib/libcurl_imp.lib" HAXX ${CURL_LIBRARIES})
if(WIN32)
	
	add_custom_command(TARGET curltest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlibd.dll ${CMAKE_BINARY_DIR}/bin
	)
endif()

target_link_libraries(curltest ${HAXX} ${ZLIB_LIBRARIES})

ADD_TEST(NAME UnitTests COMMAND unit_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_TEST(NAME CurlTest COMMAND curltest WORKING_DIRECTORY ${CMAKE_BINARY_DIR})