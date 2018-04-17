#################
# OpenKit sample
#################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

SET(SOURCES_SAMPLE 
	src/openkit-sample.cxx
	src/CommandLineArguments.cxx
)

# Add the OpenKit sample applicatation
add_executable(openkit-sample ${SOURCES_SAMPLE})

# Link the CURL and ZLIB libraries
target_link_libraries(openkit-sample ${CURLIMPLIB} ${ZLIB_LIBRARIES} OpenKit) 

# OpenKit sample currently uses the internal functions and therefore builds the sources directly.
# We therefore set the compiler flag OPENKIT_STATIC_DEFINE, which leaves any export definitions empty.
target_compile_definitions(openkit-sample PRIVATE -DOPENKIT_STATIC_DEFINE)

if(NOT WIN32)
	target_link_libraries(openkit-sample pthread)
endif()

string(REPLACE "bin/libcurl.dll" "lib/libcurl_imp.lib" HAXX ${CURL_LIBRARIES})
if(WIN32)
	
	add_custom_command(TARGET openkit-sample POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlib.dll ${CMAKE_BINARY_DIR}/bin
	)
endif()