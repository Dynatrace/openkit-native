SET(SOURCES_SAMPLE 
	src/openkit-sample.cxx
)

add_executable(openkit-sample ${SOURCES_SAMPLE})

target_link_libraries(openkit-sample LibBase)
if(NOT WIN32)
target_link_libraries(openkit-sample pthread)
endif()

string(REPLACE "bin/libcurl.dll" "lib/libcurl_imp.lib" HAXX ${CURL_LIBRARIES})
if(WIN32)
	
	add_custom_command(TARGET openkit-sample POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlibd.dll ${CMAKE_BINARY_DIR}/bin
	)
endif()