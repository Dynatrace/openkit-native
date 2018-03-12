set(PUBLIC_HEADERS
	api/OpenKit.h
)

set(SOURCES_API
	api/AbstractOpenKitBuilder.cxx
	api/DynatraceOpenKitBuilder.cxx
)

set(SOURCES_CORE
	core/util/Compressor.cxx
	core/util/CountDownLatch.cxx
	core/util/CommandLineArguments.cxx
	core/UTF8String.cxx
	core/BeaconSender.cxx
	core/OpenKit.cxx
	core/Session.cxx
	core/Action.cxx
	core/RootAction.cxx
)

set(SOURCES_COMMUNICATION
	communication/AbstractBeaconSendingState.cxx
	communication/BeaconSendingContext.cxx
	communication/BeaconSendingInitialState.cxx
	communication/BeaconSendingTerminalState.cxx
	communication/BeaconSendingRequestUtil.cxx
	communication/BeaconSendingTimeSyncState.cxx
)

set(SOURCES_PROTOCOL
	protocol/Response.cxx
	protocol/StatusResponse.cxx
	protocol/TimeSyncResponse.cxx
	protocol/HTTPClient.cxx
	protocol/ssl/SSLBlindTrustManager.cxx
	protocol/ssl/SSLStrictTrustManager.cxx
	protocol/Beacon.cxx
)

set(SOURCES_PROVIDERS
	providers/DefaultTimingProvider.cxx
	providers/DefaultHTTPClientProvider.cxx
	providers/DefaultSessionIDProvider.cxx
	providers/DefaultThreadIDProvider.cxx
)

set(SOURCES_CONFIGURATION
	configuration/HTTPClientConfiguration.cxx
	configuration/Configuration.cxx
	configuration/Device.cxx
	configuration/OpenKitType.cxx
)

# Create a combined list of all source files
set(SOURCES
	${SOURCES_API}
	${SOURCES_CORE}
	${SOURCES_COMMUNICATION}
	${SOURCES_PROVIDERS}
	${SOURCES_PROTOCOL}
	${SOURCES_CONFIGURATION}
)
# Make list of sources available in parent scope
set(SOURCES ${SOURCES} PARENT_SCOPE )

# Add the OpenKit as a library. Depending on "LIB_TYPE" either STATIC or SHARED
add_library( OpenKit ${LIB_TYPE} ${SOURCES})

if(BUILD_SHARED_LIBS)
	# A SHARED library is self-contained, thus the CURL and ZLIB libraries are linked
    target_link_libraries(OpenKit ${CURLIMPLIB} ${ZLIB_LIBRARIES})
endif()

# Create the export.h file, which includes the required #defines for SHARED or STATIC libraries
generate_export_header(OpenKit
	BASE_NAME OpenKit
	EXPORT_MACRO_NAME OPENKIT_EXPORT
	EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/OpenKit_export.h
	STATIC_DEFINE OPENKIT_STATIC_DEFINE
)

if(NOT BUILD_SHARED_LIBS)
	# For a STATIC library we set the compiler flag OPENKIT_STATIC_DEFINE
	set_target_properties(OpenKit PROPERTIES COMPILE_FLAGS -DOPENKIT_STATIC_DEFINE)
endif()

# Add a make target "install". If called (e.g. with "make install") this installs the library files
install(TARGETS OpenKit
	ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
	LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
	RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
)
# This installs the public header files and the export.h file
install(FILES
	${PUBLIC_HEADERS}
	${CMAKE_BINARY_DIR}/include/OpenKit_export.h
	DESTINATION "${INSTALL_INC_DIR}"
)
