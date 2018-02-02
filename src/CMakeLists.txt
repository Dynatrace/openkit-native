SET(SOURCES_CORE
	core/util/Compressor.cxx
	core/UTF8String.cxx
)

add_library( LibBase STATIC
	${SOURCES_CORE}
)
