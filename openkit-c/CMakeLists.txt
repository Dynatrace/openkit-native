set(SOURCES
	src/curltest.c
	src/memory.c
	src/compressor.c
)

add_executable(
    curltest 
        ${SOURCES}
)

string(REPLACE "bin/libcurl.dll" "lib/libcurl_imp.lib" HAXX ${CURL_LIBRARIES})
if(WIN32)
	add_custom_command(TARGET curltest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlibd.dll ${CMAKE_BINARY_DIR}/bin
	)
endif()

target_link_libraries(curltest ${HAXX} ${ZLIB_LIBRARIES})
