if(WIN32)
	set(SOURCES_PLATFORM 
		src/base/platform/windows/threading_utils_rwlock_win32.c
		src/base/platform/windows/threading_utils_thread_win32.c
		src/base/platform/windows/threading_utils_mutex_win32.c
		src/base/platform/windows/threading_utils_condition_variable_win32.c
		src/base/platform/windows/threading_utils_atomic_win32.c
	)
else()
	set(SOURCES_PLATFORM 
		src/base/platform/posix/threading_utils_thread_posix.c
		src/base/platform/posix/threading_utils_rwlock_posix.c
		src/base/platform/posix/threading_utils_mutex_posix.c
		src/base/platform/posix/threading_utils_condition_variable_posix.c
		src/base/platform/posix/threading_utils_atomic_posix.c
	)
endif()

SET(SOURCES_COMMON
	src/compressor.c
	src/memory.c
	src/utf8string.c
)

add_library( LibBase STATIC
	${SOURCES_PLATFORM}
	${SOURCES_COMMON}
)

add_subdirectory(test)
