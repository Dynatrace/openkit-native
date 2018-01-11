set(SOURCES_CURLTEST
	src/compressor.c
	src/memory.c
	test/curltest.c
)

set(SOURCES_RWLOCKTEST
	src/memory.c
	test/rwlock_test.c
)

set(SOURCES_MUTEXTEST
	src/memory.c
	test/mutex_test.c
)

set(SOURCES_CONDITIONVARIABLETEST
	src/memory.c
	test/condition_variable_test.c
)


if(WIN32)
	set(SOURCES_PLATFORM 
		src/base/platform/windows/threading_utils_rwlock_win32.c
		src/base/platform/windows/threading_utils_thread_win32.c
		src/base/platform/windows/threading_utils_mutex_win32.c
		src/base/platform/windows/threading_utils_condition_variable_win32.c
	)
else()
	set(SOURCES_PLATFORM 
		src/base/platform/posix/threading_utils_thread_posix.c
		src/base/platform/posix/threading_utils_rwlock_posix.c
		src/base/platform/posix/threading_utils_mutex_posix.c
		src/base/platform/posix/threading_utils_condition_variable_posix.c
	)
endif()


add_executable( curltest 
    ${SOURCES_CURLTEST}
	${SOURCES_PLATFORM}
)

add_executable(	rwlock_test
	${SOURCES_RWLOCKTEST}
	${SOURCES_PLATFORM}
)

add_executable(	mutex_test
	${SOURCES_MUTEXTEST}
	${SOURCES_PLATFORM}
)

add_executable(	condition_variable_test
	${SOURCES_CONDITIONVARIABLETEST}
	${SOURCES_PLATFORM}
)

string(REPLACE "bin/libcurl.dll" "lib/libcurl_imp.lib" HAXX ${CURL_LIBRARIES})
if(WIN32)
	
	add_custom_command(TARGET curltest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib-1.2.11/bin/zlibd.dll ${CMAKE_BINARY_DIR}/bin
	)
endif()

target_link_libraries(curltest ${HAXX} ${ZLIB_LIBRARIES})
