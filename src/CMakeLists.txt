SET(SOURCES_CORE
	core/util/Compressor.cxx
	core/util/CountDownLatch.cxx
	core/util/CommandLineArguments.cxx
	core/UTF8String.cxx
	core/BeaconSender.cxx
)

SET(SOURCES_COMMUNICATION
	communication/AbstractBeaconSendingState.cxx
	communication/BeaconSendingContext.cxx
	communication/BeaconSendingInitialState.cxx
	communication/BeaconSendingTerminalState.cxx
	communication/BeaconSendingRequestUtil.cxx
	communication/BeaconSendingTimeSyncState.cxx
)

SET(SOURCES_PROTOCOL
	protocol/Response.cxx
	protocol/StatusResponse.cxx
	protocol/TimeSyncResponse.cxx
	protocol/HTTPClient.cxx
	protocol/ssl/SSLBlindTrustManager.cxx
	protocol/ssl/SSLStrictTrustManager.cxx
)

SET(SOURCES_PROVIDERS
	providers/DefaultTimingProvider.cxx
	providers/DefaultHTTPClientProvider.cxx
	providers/DefaultSessionIDProvider.cxx
)

SET(SOURCES_CONFIGURATION
	configuration/HTTPClientConfiguration.cxx
	configuration/Configuration.cxx
	configuration/Device.cxx
	configuration/OpenKitType.cxx
)

add_library( LibBase STATIC
	${SOURCES_CORE}
	${SOURCES_COMMUNICATION}
	${SOURCES_PROVIDERS}
	${SOURCES_PROTOCOL}
	${SOURCES_CONFIGURATION}
)
