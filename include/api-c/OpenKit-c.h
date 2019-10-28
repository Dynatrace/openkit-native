/**
* Copyright 2018-2019 Dynatrace LLC
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef _API_C_OPENKIT_H
#define _API_C_OPENKIT_H

#include <stdbool.h>
#include <stdint.h>
#include "curl/curl.h"

#include "OpenKit_export.h"

#ifdef __cplusplus
extern "C" {
#endif

	//--------------
	//  Logger
	//--------------

	//
	// Note:
	// The OpenKit comes with a default logger printing trace statements to stdout. 
	// This default logger can be replaced with a user provided logger. To achieve this,
	// the OpenKit user must call @ref createLogger and provide two function pointers:
	// One funtion pointer to a check function if the provided log level is enabled and
	// another function pointer to the actual log function performing the log writing.
	//

	typedef enum LOG_LEVEL
	{
		LOGLEVEL_DEBUG = 0,
		LOGLEVEL_INFO  = 1,
		LOGLEVEL_WARN  = 2,
		LOGLEVEL_ERROR = 3
	} LOG_LEVEL;

	static const char* LOG_LEVEL_STRINGS[] =
	{
		"DEBUG",
		"INFO",
		"WARN",
		"ERROR"
	};

	static inline const char* toString(enum LOG_LEVEL level)
	{
		return LOG_LEVEL_STRINGS[level];
	}

	/// Function to check if the provided log level is enabled. Called before each trace statement
	typedef bool (*levelEnabledFunc)(LOG_LEVEL /* logLevel */);

	/// Function to perform the log. The 
	typedef void (*logFunc)(LOG_LEVEL /* logLevel */, const char* /* traceStatement */);
	
	/// An opaque type that we'll use as a handle
	struct LoggerHandle;

	OPENKIT_EXPORT struct LoggerHandle* createLogger(levelEnabledFunc levelEnabledFunc, logFunc logFunc);
	OPENKIT_EXPORT void destroyLogger(struct LoggerHandle* loggerHandle);


	//--------------
	//  TrustManager
	//--------------

	typedef enum TRUST_MODE
	{
		STRICT_TRUST = 0,	///< Use a class implementing @ref openkit::ISSLTrustManager, trusting only valid certificates
		BLIND_TRUST  = 1,	///< Use a class implementing @ref openkit::ISSLTrustManager, blindly trusting every certificate and every host (not recommended!)
		CUSTOM_TRUST = 2	///< Use a custom SSL trustmanager, which is provided via the @c trustManagerHandle returned from @ref createCustomTrustManager
	} TRUST_MODE;

	/// Function to apply the trust manager configuration on the passed CURL handle
	typedef void(*applyTrustManagerFunc)(CURL* /* curl */);

	/// An opaque type that we'll use as a handle
	struct TrustManagerHandle;

	/// Create a custom trust manager. With this trust manager the most fine-granular trust configuration can be achieved.
	OPENKIT_EXPORT struct TrustManagerHandle* createCustomTrustManager(applyTrustManagerFunc applyTrustManagerFunc);

	/// destroy a trust manager
	OPENKIT_EXPORT void destroyTrustManager(struct TrustManagerHandle* trustManagerHandle);
	
	//--------------
	//  Configuration
	//--------------

	typedef enum DataCollectionLevel
	{
		DATA_COLLECTION_LEVEL_OFF = 0,
		DATA_COLLECTION_LEVEL_PERFORMANCE = 1,
		DATA_COLLECTION_LEVEL_USER_BEHAVIOR = 2,
		DATA_COLLECTION_LEVEL_COUNT
	} DataCollectionLevel;

	typedef enum CrashReportingLevel
	{
		CRASH_REPORTING_LEVEL_OFF = 0,
		CRASH_REPORTING_LEVEL_OPT_OUT_CRASHES = 1,
		CRASH_REPORTING_LEVEL_OPT_IN_CRASHES = 2,
		CRASH_REPORTING_LEVEL_COUNT
	} CrashReportingLevel;

	/// an opaque type that we'll use as a handle
	struct OpenKitConfigurationHandle;

	///
	/// Creates an OpenKit configuration object
	/// @param[in] endpointURL endpoint OpenKit connects to
	/// @param[in] applicationID unique application id
	/// @param[in] deviceID unique device id
	/// @returns a configuration object that can be used for both AppMon and Dynatrace OpenKit instances
	///
	OPENKIT_EXPORT struct OpenKitConfigurationHandle* createOpenKitConfiguration(const char* endpointURL, const char* applicationID, int64_t deviceID);

	///
	/// Creates an OpenKit configuration object with @c deviceID given as string.
	/// @remarks If the given @c deviceID is longer than 250 characters, only the first 250 characters are used.
	/// @param[in] endpointURL endpoint OpenKit connects to
	/// @param[in] applicationID unique application id
	/// @param[in] deviceID unique device id
	/// @returns a configuration object that can be used for both AppMon and Dynatrace OpenKit instances
	///
	OPENKIT_EXPORT struct OpenKitConfigurationHandle* createOpenKitConfigurationWithStringDeviceID(const char* endpointURL, const char* applicationID, const char* deviceID);

	///
	/// Destroys a given configuration
	/// @param[in] configurationHandle configuration handle to clean up
	///
	OPENKIT_EXPORT void destroyOpenKitConfiguration(struct OpenKitConfigurationHandle* configurationHandle);

	///
	/// Set the logger handle in the OpenKit configuration
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] loggerHandle optional parameter to provide a logger that shall be used. If @c NULL is provided the DefaultLogger is used.
	///
	OPENKIT_EXPORT void useLoggerForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, struct LoggerHandle* loggerHandle);

	///
	/// Set the application version in the OpenKit configuration
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] applicationVersion optional parameter, the application version. If @c NULL is provided the default application version is used.
	///
	OPENKIT_EXPORT void useApplicationVersionForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* applicationVersion);

	///
	/// Set the application name in the OpenKit configuration
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] applicationName optional name for the application. If @c NULL is provided the application name is an empty string.
	///
	OPENKIT_EXPORT void useApplicationNameForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* applicationName);

	///
	/// Set the trust mode and trust manager handle in the OpenKit configuration
	/// Default mode is STRICT_TRUST which does not need a custom handle
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] trustMode required parameter which trust manager shall be used. Recommended is @c STRICT_TRUST or for fine-granular @c CUSTOM_TRUST.
	/// @param[in] trustManagerHandle required parameter if the @c trustMode @c CUSTOM_TRUST is provided. Ignored for the other trust modes.
	///
	OPENKIT_EXPORT void useTrustModeForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, TRUST_MODE trustMode, struct TrustManagerHandle* trustManagerHandle);

	///
	/// Set the operating system name in the OpenKit configuration
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] operatingSystem optional parameter, name of the operating system. If @c NULL is provided the default operating system is used.
	///
	OPENKIT_EXPORT void useOperatingSystemForConfiguration(struct OpenKitConfigurationHandle* configurationHandle,const char* operatingSystem);

	///
	/// Set the manufacturer in the OpenKit configuration
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] manufacturer optional parameter, manufacturer of the device. If @c NULL is provided the default manufacturer is used.
	///
	OPENKIT_EXPORT void useManufacturerForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* manufacturer);

	///
	/// Set the model id in the OpenKit configuration
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] modelID optional parameter, model version or id of the device. If @c NULL the default model ID is used.
	///
	OPENKIT_EXPORT void useModelIDForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* modelID);

	///
	/// Set the behavior of the beacon cache in the OpenKit configuration
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] beaconCacheMaxRecordAge optional parameter, maximum age of cache records. A value of -1 will lead to the default value. All positive integers starting with 0 are valid.
	/// @param[in] beaconCacheLowerMemoryBoundary optional parameter, lower memory boundary for beacon cache. A value of -1 will lead to the default value. All positive integers starting with 0 are valid.
	/// @param[in] beaconCacheUpperMemoryBoundary optional parameter, upper memory boundary for beacon cache. A value of -1 will lead to the default value. All positive integers starting with 0 are valid.
	///
	OPENKIT_EXPORT void useBeaconCacheBehaviorForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, int64_t beaconCacheMaxRecordAge, int64_t beaconCacheLowerMemoryBoundary, int64_t beaconCacheUpperMemoryBoundary);

	///
	/// Set the data collection level in the OpenKit configuration
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] dataCollectionLevel optional parameter, default level if USER_BEHAVIOR
	///
	OPENKIT_EXPORT void useDataCollectionLevelForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, DataCollectionLevel dataCollectionLevel);

	///
	/// Set the crash reporting level in the OpenKit configuration
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] crashReportingLevel optional parameter, default behavior is OPT_IN_CRASHES
	///
	OPENKIT_EXPORT void useCrashReportingLevelForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, CrashReportingLevel crashReportingLevel);

	//--------------
	//  OpenKit
	//--------------

	/// An opaque type that we'll use as a handle
	struct OpenKitHandle;

	///
	/// Creates an OpenKit instance for Dynatrace Saas/Managed 
	/// @param[in] configurationHandle configuration object
	/// @return OpenKit instance handle to work with
	///
	OPENKIT_EXPORT struct OpenKitHandle* createDynatraceOpenKit(struct OpenKitConfigurationHandle* configurationHandle );

	///
	/// Creates an OpenKit instance for AppMon
	/// @param[in] configurationHandle configuration object
	/// @return OpenKit instance handle to work with
	///
	OPENKIT_EXPORT struct OpenKitHandle* createAppMonOpenKit(struct OpenKitConfigurationHandle* configurationHandle);

	///
	/// Shuts down the OpenKit, ending all open Sessions and waiting for them to be sent.
	/// After calling @c shutdown the openKitHandle is released and must not be used any more.
	/// @param[in] openKitHandle the handle returned by @ref createDynatraceOpenKit or @ref createAppMonOpenKit
	///
	OPENKIT_EXPORT void shutdownOpenKit(struct OpenKitHandle* openKitHandle);

	///
	/// Waits until OpenKit is fully initialized.
	///
	/// The calling thread is blocked until OpenKit is fully initialized or until OpenKit is shut down using the
	/// shutdown() method.
	/// 
	/// Be aware, if @ref openkit::AbstractOpenKitBuilder is wrongly configured, for example when creating an
	/// instance with an incorrect endpoint URL, then this method might hang indefinitely, unless shutdown() is called.
	///
	/// @param[in] openKitHandle the handle returned by @ref createDynatraceOpenKit or @ref createAppMonOpenKit
	/// @returns @c true when OpenKit is fully initialized, @c false when a shutdown request was made.
	///
	OPENKIT_EXPORT bool waitForInitCompletion(struct OpenKitHandle* openKitHandle);

	///
	/// Waits until OpenKit is fully initialized or the given timeout expired
	///
	/// The calling thread is blocked until OpenKit is fully initialized or until OpenKit is shut down using the
	/// shutdown() method or the timeout expired.
	/// 
	/// Be aware, if @ref openkit::AbstractOpenKitBuilder is wrongly configured, for example when creating an
	/// instance with an incorrect endpoint URL, then this method might hang indefinitely, unless shutdown() is called
	/// or timeout expires.
	///
	/// @param[in] openKitHandle the handle returned by @ref createDynatraceOpenKit or @ref createAppMonOpenKit
	/// @param[in] timeoutMillis The maximum number of milliseconds to wait for initialization being completed.
	/// @returns @c true when OpenKit is fully initialized, @c false when a shutdown request was made or @c timeoutMillis expired.
	///
	OPENKIT_EXPORT bool waitForInitCompletionWithTimeout(struct OpenKitHandle* openKitHandle, int64_t timeoutMillis);

	///
	/// Returns whether OpenKit is initialized or not.
	/// @param[in] openKitHandle the handle returned by @ref createDynatraceOpenKit or @ref createAppMonOpenKit
	/// @returns @c true if OpenKit is fully initialized, @c false if OpenKit still performs initialization.
	///
	OPENKIT_EXPORT bool isInitialized(struct OpenKitHandle* openKitHandle);


	//--------------
	//  Session
	//--------------

	/// An opaque type that we'll use as a handle
	struct SessionHandle;

	///
	/// Creates a session instance which can then be used to create actions.
	/// @param[in] openKitHandle   the handle returned by @ref createDynatraceOpenKit or @ref createAppMonOpenKit
	/// @param[in] clientIPAddress client IP address where this Session is coming from
	/// @return Session instance handle to work with
	///
	OPENKIT_EXPORT struct SessionHandle* createSession(struct OpenKitHandle* openKitHandle, const char* clientIPAddress);

	///
	/// Creates a session instance which can then be used to create actions.
	///
	/// This is similar to the function \see ::createSession(struct OpenKitHandle*, const char*), except that
	/// the client's IP address is determined on the server side.
	///
	/// @param[in] openKitHandle   the handle returned by @ref createDynatraceOpenKit or @ref createAppMonOpenKit
	/// @return Session instance handle to work with
	///
	OPENKIT_EXPORT struct SessionHandle* createSessionWithAutoIpDetermination(struct OpenKitHandle* openKitHandle);

	///
	/// Ends this session and marks it as ready for immediate sending.
	/// After calling @c end the sessionHandle is released and must not be used any more.
	/// @param[in] sessionHandle the handle returned by @ref createSession
	///
	OPENKIT_EXPORT void endSession(struct SessionHandle* sessionHandle);

	///
	/// Tags a session with the provided @c userTag.
	/// If the given @c userTag is @c NULL or an empty string,
	/// no user identification will be reported to the server.
	/// @param[in] sessionHandle the handle returned by @ref createSession
	/// @param[in] userTag       id of the user
	///
	OPENKIT_EXPORT void identifyUser(struct SessionHandle* sessionHandle, const char* userTag);

	///
	/// Reports a crash with a specified error name, crash reason and a stacktrace.
	/// Note: If the given @c errorName is @c NULL or an empty string, no crash report will be sent to the server.
	/// @param[in] sessionHandle the handle returned by @ref createSession
	/// @param[in] errorName     name of the error leading to the crash (e.g. Exception class)
	/// @param[in] reason        reason or description of that error
	/// @param[in] stacktrace    stacktrace leading to that crash
	///
	OPENKIT_EXPORT void reportCrash(struct SessionHandle* sessionHandle, const char* errorName, const char* reason, const char* stacktrace);

	//--------------
	//  Root Action
	//--------------

	/// An opaque type that we'll use as a handle
	struct RootActionHandle;
	
	///
	/// Enters a root action with a specified name in this session.
	/// @param[in] sessionHandle the handle returned by @ref createSession
	/// @param[in] rootActionName    name of the Action
	/// @returns Root action instance to work with
	///
	OPENKIT_EXPORT struct RootActionHandle* enterRootAction(struct SessionHandle* sessionHandle, const char* rootActionName);

	///
	/// Leaves this root action.
	/// @param[in] rootActionHandle the handle returned by @ref enterRootAction
	///
	OPENKIT_EXPORT void leaveRootAction(struct RootActionHandle* rootActionHandle);

	///
	/// Reports an event with a specified name (but without any value).
	///
	/// If given @c eventName is @c NULL then no event is reported to the system.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] eventName		name of the event
	///
	OPENKIT_EXPORT void reportEventOnRootAction(struct RootActionHandle* rootActionHandle, const char* eventName);

	///
	/// Reports an int value with a specified name.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] valueName		name of this value
	/// @param[in] value			value itself
	///
	OPENKIT_EXPORT void reportIntValueOnRootAction(struct RootActionHandle* rootActionHandle, const char* valueName, int32_t value);

	///
	/// Reports a double value with a specified name.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] valueName		name of this value
	/// @param[in] value			value itself
	///
	OPENKIT_EXPORT void reportDoubleValueOnRootAction(struct RootActionHandle* rootActionHandle, const char* valueName, double value);

	///
	/// Reports a String value with a specified name.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] valueName		name of this value
	/// @param[in] value			value itself
	///
	OPENKIT_EXPORT void reportStringValueOnRootAction(struct RootActionHandle* rootActionHandle, const char* valueName, const char* value);

	///
	/// Reports an error with a specified name, error code and reason.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] errorName		name of this error
	/// @param[in] errorCode		numeric error code of this error
	/// @param[in] reason			reason for this error
	///
	OPENKIT_EXPORT void reportErrorOnRootAction(struct RootActionHandle* rootActionHandle, const char* errorName, int32_t errorCode, const char* reason);

	//--------------
	//  Action
	//--------------

	/// An opaque type that we'll use as a handle
	struct ActionHandle;

	///
	/// Enters an action with a specified name in this root action.
	/// @param[in] rootActionHandle the handle returned by @ref enterRootAction
	/// @param[in] actionName       name of the Action
	/// @returns Root action instance to work with
	///
	OPENKIT_EXPORT struct ActionHandle* enterAction(struct RootActionHandle* rootActionHandle, const char* actionName);

	///
	/// Leaves this action.
	/// @param[in] actionHandle the handle returned by @ref enterAction
	///
	OPENKIT_EXPORT void leaveAction(struct ActionHandle* actionHandle);

	///
	/// Reports an event with a specified name (but without any value).
	///
	/// If given @c eventName is @c NULL then no event is reported to the system.
	///
	/// @param[in] actionHandle	the handle returned by @ref enterAction
	/// @param[in] eventName	name of the event
	///
	OPENKIT_EXPORT void reportEventOnAction(struct ActionHandle* actionHandle, const char* eventName);

	///
	/// Reports an int value with a specified name.
	///
	/// @param[in] actionHandle	the handle returned by @ref enterAction
	/// @param[in] valueName	name of this value
	/// @param[in] value		value itself
	///
	OPENKIT_EXPORT void reportIntValueOnAction(struct ActionHandle* actionHandle, const char* valueName, int32_t value);

	///
	/// Reports a double value with a specified name.
	///
	/// @param[in] actionHandle	the handle returned by @ref enterAction
	/// @param[in] valueName	name of this value
	/// @param[in] value		value itself
	///
	OPENKIT_EXPORT void reportDoubleValueOnAction(struct ActionHandle* actionHandle, const char* valueName, double value);

	///
	/// Reports a String value with a specified name.
	///
	/// @param[in] actionHandle	the handle returned by @ref enterAction
	/// @param[in] valueName	name of this value
	/// @param[in] value		value itself
	///
	OPENKIT_EXPORT void reportStringValueOnAction(struct ActionHandle* actionHandle, const char* valueName, const char* value);

	///
	/// Reports an error with a specified name, error code and reason.
	///
	/// @param[in] actionHandle	the handle returned by @ref enterAction
	/// @param[in] errorName	name of this error
	/// @param[in] errorCode	numeric error code of this error
	/// @param[in] reason		reason for this error
	///
	OPENKIT_EXPORT void reportErrorOnAction(struct ActionHandle* actionHandle, const char* errorName, int32_t errorCode, const char* reason);


	//--------------------
	//  Webrequest Tracer
	//--------------------

#define WEBREQUEST_TAG_HEADER "X-dynaTrace"

	/// An opaque type that we'll use as a handle
	struct WebRequestTracerHandle;

	///
	/// Allows tracing and timing of a web request handled by any 3rd party HTTP Client (e.g. CURL, EasyHttp, ...).
	/// In this case the Dynatrace HTTP header (@ref WEBREQUEST_TAG_HEADER) has to be set manually to the
	/// tag value of this WebRequestTracer. <br>
	/// If the web request is continued on a server-side Agent (e.g. Java, .NET, ...) this Session will be correlated to
	/// the resulting server-side PurePath.
	///
	/// @param[in] sessionHandle	the handle returned by @ref createSession
	/// @param[in] url				the URL of the web request to be tagged and timed
	/// @return a WebRequestTracer which allows getting the tag value and adding timing information
	///
	OPENKIT_EXPORT struct WebRequestTracerHandle* traceWebRequestOnSession(struct SessionHandle* sessionHandle, const char* url);

	///
	/// Allows tracing and timing of a web request handled by any 3rd party HTTP Client (e.g. CURL, EasyHttp, ...).
	/// In this case the Dynatrace HTTP header (@ref WEBREQUEST_TAG_HEADER) has to be set manually to the
	/// tag value of this WebRequestTracer. <br>
	/// If the web request is continued on a server-side Agent (e.g. Java, .NET, ...) this Session will be correlated to
	/// the resulting server-side PurePath.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] url				the URL of the web request to be tagged and timed
	/// @return a WebRequestTracer which allows getting the tag value and adding timing information
	///
	OPENKIT_EXPORT struct WebRequestTracerHandle* traceWebRequestOnRootAction(struct RootActionHandle* rootActionHandle, const char* url);

	///
	/// Allows tracing and timing of a web request handled by any 3rd party HTTP Client (e.g. CURL, EasyHttp, ...).
	/// In this case the Dynatrace HTTP header (@ref WEBREQUEST_TAG_HEADER) has to be set manually to the
	/// tag value of this WebRequestTracer. <br>
	/// If the web request is continued on a server-side Agent (e.g. Java, .NET, ...) this Session will be correlated to
	/// the resulting server-side PurePath.
	///
	/// @param[in] actionHandle	the handle returned by @ref enterAction
	/// @param[in] url			the URL of the web request to be tagged and timed
	/// @return a WebRequestTracer which allows getting the tag value and adding timing information
	///
	OPENKIT_EXPORT struct WebRequestTracerHandle* traceWebRequestOnAction(struct ActionHandle* actionHandle, const char* url);

	///
	/// Starts the web request timing. Should be called when the web request is initiated.
	/// @param[in] webRequestTracerHandle the handle returned by @ref traceWebRequestOnRootAction or @ref traceWebRequestOnAction
	///
	OPENKIT_EXPORT void startWebRequest(struct WebRequestTracerHandle* webRequestTracerHandle);

	///
	/// Stops the web request timing. Should be called when the web request is finished.
	/// After calling @c stop the webRequestTracerHandle is released and must not be used any more.
	/// @param[in] webRequestTracerHandle the handle returned by @ref traceWebRequestOnRootAction or @ref traceWebRequestOnAction
	///
	OPENKIT_EXPORT void stopWebRequest(struct WebRequestTracerHandle* webRequestTracerHandle);

	///
	/// Returns the Dynatrace tag which has to be set manually as Dynatrace HTTP header
	/// ( @ref OpenKitConstants::WEBREQUEST_TAG_HEADER).
	/// This is only necessary for tracing web requests via 3rd party HTTP clients.
	///
	/// @param[in] webRequestTracerHandle the handle returned by @ref traceWebRequestOnRootAction or @ref traceWebRequestOnAction
	/// @returns the Dynatrace tag to be set as HTTP header value or an empty String if capture is off
	///
	OPENKIT_EXPORT const char* getTag(struct WebRequestTracerHandle* webRequestTracerHandle);

	///
	/// Sets the response code of this web request. Has to be called before@ref stopWebRequest().
	///
	/// @param[in] webRequestTracerHandle the handle returned by @ref traceWebRequestOnRootAction or @ref traceWebRequestOnAction
	/// @param[in] responseCode response code of this web request
	///
	OPENKIT_EXPORT void setResponseCode(struct WebRequestTracerHandle* webRequestTracerHandle, int32_t responseCode);

	///
	/// Sets the amount of sent data of this web request. Has to be called before @ref stopWebRequest().
	///
	/// @param[in] webRequestTracerHandle the handle returned by @ref traceWebRequestOnRootAction or @ref traceWebRequestOnAction
	/// @param[in] bytesSent number of bytes sent
	///
	OPENKIT_EXPORT void setBytesSent(struct WebRequestTracerHandle* webRequestTracerHandle, int32_t bytesSent);

	///
	/// Sets the amount of received data of this web request. Has to be called before @ref stopWebRequest().
	///
	/// @param[in] webRequestTracerHandle the handle returned by @ref traceWebRequestOnRootAction or @ref traceWebRequestOnAction
	/// @param[in] bytesReceived number of bytes received
	///
	OPENKIT_EXPORT void setBytesReceived(struct WebRequestTracerHandle* webRequestTracerHandle, int32_t bytesReceived);


#ifdef __cplusplus
}  // extern "C"
#endif

#endif
