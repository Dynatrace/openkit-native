set(SOURCES
	src/compressor.c
	src/memory.c
	test/curltest.c
)

#if(WIN32)
#	set(SOURCES "${SOURCES};src/base/platform/windows/threading_utils_win32.c")
#else()
#	set(SOURCES "${SOURCES};src/base/platform/posix/threading_utils_posix.c")
#endif()


add_executable(
    curltest 
        ${SOURCES}
)

if(WIN32)
	string(REPLACE "bin/libcurl.dll" "lib/libcurl_imp.lib" HAXX ${CURL_LIBRARIES})
	add_custom_command(TARGET curltest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlibd.dll ${CMAKE_BINARY_DIR}/bin
	)
endif()

target_link_libraries(curltest ${HAXX} ${ZLIB_LIBRARIES})
