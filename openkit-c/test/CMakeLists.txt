include_directories(${GTEST_INCLUDE_DIRS})

set(SOURCES_CURLTEST
	curltest.c
)

set(SOURCES_RWLOCKTEST
	rwlock_test.c
)

set(SOURCES_MUTEXTEST
	mutex_test.c
)

set(SOURCES_CONDITIONVARIABLETEST
	condition_variable_test.c
)

set(SOURCES_UNITTEST
	atomic_test.cxx
	utf8stringtest.cxx
)


add_executable( curltest 
    ${SOURCES_CURLTEST}
)

target_link_libraries(curltest LibBase)

add_executable(	rwlock_test
	${SOURCES_RWLOCKTEST}
)

target_link_libraries(rwlock_test LibBase)

add_executable(	mutex_test
	${SOURCES_MUTEXTEST}
)

target_link_libraries(mutex_test LibBase)

add_executable(	condition_variable_test
	${SOURCES_CONDITIONVARIABLETEST}
)

target_link_libraries(condition_variable_test LibBase)

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
ADD_TEST(NAME ConditionVariableTest COMMAND condition_variable_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_TEST(NAME RwLockTest COMMAND rwlock_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_TEST(NAME MutexTest COMMAND mutex_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_TEST(NAME CurlTest COMMAND curltest WORKING_DIRECTORY ${CMAKE_BINARY_DIR})