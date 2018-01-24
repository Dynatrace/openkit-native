SET(SOURCES_COMMON
	src/compressor.cxx
	src/utf8string.cxx
)

add_library( LibBase STATIC
	${SOURCES_COMMON}
)

add_subdirectory(test)
