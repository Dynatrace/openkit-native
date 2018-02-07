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

SET(SOURCES_PROTOCOL
	protocol/Response.cxx
	protocol/StatusResponse.cxx
	protocol/HTTPClient.cxx
)

SET(SOURCES_PROVIDERS
	providers/DefaultTimingProvider.cxx
)

add_library( LibBase STATIC
	${SOURCES_CORE}
	${SOURCES_COMMUNICATION}
	${SOURCES_PROVIDERS}
	${SOURCES_PROTOCOL}
)
