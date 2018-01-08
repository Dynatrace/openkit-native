set(SOURCES
	src/curltest.c
)

add_executable(
    curltest 
        ${SOURCES}
)

string(REPLACE "bin/libcurl.dll" "lib/libcurl_imp.lib" HAXX ${CURL_LIBRARIES})

add_custom_command(TARGET curltest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_BINARY_DIR}/bin
)

target_link_libraries(curltest ${HAXX})
