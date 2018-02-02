SET(SOURCES_CORE
	core/util/Compressor.cxx
)

add_library( LibBase STATIC
	${SOURCES_CORE}
)
