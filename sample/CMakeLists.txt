#################
# OpenKit sample
#################

# The openkit-sample currently does not use the public OpenKit-API, but instead it uses internal functions.
# If the OpenKit is built as a shared library, we don't want to export all (internal) symbols.
# Therefore, for the target "openkit-sample", the sources are directly accessed and compiled.
# This is done by making the list of sources available here and creating the releative path with prepend.
set(SOURCES_FOR_OPENKIT_SAMPLE)
prepend_to_list(SOURCES_FOR_OPENKIT_SAMPLE ../src/ ${SOURCES})

SET(SOURCES_SAMPLE 
	src/openkit-sample.cxx
	${SOURCES_FOR_OPENKIT_SAMPLE}
)

# Add the OpenKit sample applicatation
add_executable(openkit-sample ${SOURCES_SAMPLE})

# Link the CURL and ZLIB libraries
target_link_libraries(openkit-sample ${CURLIMPLIB} ${ZLIB_LIBRARIES})

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