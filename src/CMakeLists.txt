SET(SOURCES_CORE
	core/util/Compressor.cxx
	core/UTF8String.cxx
	core/BeaconSender.cxx
)

SET(SOURCES_COMMUNICATION
	communication/BeaconSendingContext.cxx
	communication/BeaconSendingInitialState.cxx
	communication/BeaconSendingTerminalState.cxx
)

add_library( LibBase STATIC
	${SOURCES_CORE}
	${SOURCES_COMMUNICATION}
)
