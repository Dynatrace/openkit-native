###################
# OpenKit sample-c
###################

SET(SOURCES_SAMPLE2 
	src/openkit-sample.c
)

# Add the OpenKit sample applicatation
add_executable(openkit-sample-c ${SOURCES_SAMPLE2})

# Link the CURL and ZLIB libraries
target_link_libraries(openkit-sample-c OpenKit ${CURLIMPLIB} ${ZLIB_LIBRARIES})

# OpenKit sample currently uses the internal functions and therefore builds the sources directly.
# We therefore set the compiler flag OPENKIT_STATIC_DEFINE, which leaves any export definitions empty.
target_compile_definitions(openkit-sample-c PRIVATE -DOPENKIT_STATIC_DEFINE)

if(NOT WIN32)
	target_link_libraries(openkit-sample-c pthread)
endif()

string(REPLACE "bin/libcurl.dll" "lib/libcurl_imp.lib" HAXX ${CURL_LIBRARIES})
if(WIN32)
	
	add_custom_command(TARGET openkit-sample-c POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlib.dll ${CMAKE_BINARY_DIR}/bin
	)
endif()