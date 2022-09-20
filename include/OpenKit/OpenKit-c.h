/**
 * Copyright 2018-2021 Dynatrace LLC
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

#include "OpenKit/OpenKitExports.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

	// copy typedef from curl.h so that we don't need the transitive dependency
	typedef void CURL;

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

	//-------------------------------------------------
	// Single linked list storing const char* as values
	//-------------------------------------------------

	///
	/// Single linked list with string values
	///
	typedef struct OpenKitSList
	{
		/// Pointer to the next list node or @c NULL if it's the last node
		struct OpenKitSList* next;

		/// Pointer to the value
		const char* value;

	} OpenKitSList;

	///
	/// Destroys an OpenKitSList*
	///
	/// @remarks After calling this function the passed argument @c sValueList is no longer valid.
	///
	/// @param[in] sValueList The OpenKitSList to destroy
	///
	OPENKIT_EXPORT void destroyOpenKitSList(struct OpenKitSList* sValueList);

	//--------------------------------------
	//  HTTP request & response interception
	//--------------------------------------
	
	/// C wrapper for openkit::IHttpRequest
	struct OpenKitHttpRequest;

	///
	/// Get the HTTP request URI that is used by OpenKit to communicate with the backend.
	/// 
	/// @remarks Return value is valid within openKitInterceptHttpRequestFunc and must not be freed.
	///
	/// @param[in] openKitHttpRequest OpenKit HTTP request struct
	/// @return HTTP request URI
	///
	OPENKIT_EXPORT const char* getOpenKitHttpRequestUri(const struct OpenKitHttpRequest* openKitHttpRequest);

	///
	/// Get the HTTP request method that is used by OpenKit to communicate with the backend.
	/// 
	/// @remarks Return value is valid within openKitInterceptHttpRequestFunc and must not be freed.
	///
	/// @param[in] openKitHttpRequest OpenKit HTTP request struct
	/// @return HTTP request method as const char*.
	///
	OPENKIT_EXPORT const char* getOpenKitHttpRequestMethod(const struct OpenKitHttpRequest* openKitHttpRequest);

	///
	/// Get a list containing all header names that have been set so far on the HTTP request.
	/// 
	/// @remarks The returned list must be destroyed using the destroyOpenKitSList function, otherwise memory leaks occur.
	/// 
	/// @param[in] openKitHttpRequest OpenKit HTTP request struct
	/// @return List containing all request header names
	///
	OPENKIT_EXPORT struct OpenKitSList* getOpenKitHttpRequestHeaderNames(const struct OpenKitHttpRequest* openKitHttpRequest);

	///
	/// Get a boolean indicating whether the HTTP request contains the header with given @c name or not.
	///
	/// @param[in] openKitHttpRequest OpenKit HTTP request struct
	/// @param[in] name The name of the HTTP request header
	/// @return @c true if request header with given @c name is contained, @c false otherwise.
	///
	OPENKIT_EXPORT bool containsOpenKitHttpRequestHeader(const struct OpenKitHttpRequest* openKitHttpRequest, const char* name);

	///
	/// Get the value of an HTTP request header.
	/// 
	/// @remarks Return value is valid within openKitInterceptHttpRequestFunc and must not be freed.
	///
	/// @param[in] openKitHttpRequest OpenKit HTTP request struct
	/// @param[in] name The name of the HTTP request header
	/// @return Header value for given header name, or @c NULL if header is not contained.
	///
	OPENKIT_EXPORT const char* getOpenKitHttpRequestHeader(const struct OpenKitHttpRequest* openKitHttpRequest, const char* name);

	///
	/// Sets an HTTP header or overwrites an existing HTTP header with new value.
	/// 
	/// @par
	/// For compatibility reasons the following headers are restricted and cannot be set.
	/// - @c Access-Control-Request-Headers
	/// - @c Access-Control-Request-Method
	/// - @c Connection
	/// - @c Content-Length
	/// - @c Content-Transfer-Encoding
	/// - @c Host
	/// - @c Keep-Alive
	/// - @c Origin
	/// - @c Trailer
	/// - @c Transfer-Encoding
	/// - @c Upgrade
	/// - @c Via
	///
	/// @param[in] openKitHttpRequest OpenKit HTTP request struct
	/// @param[in] name The name of the HTTP request header
	/// @param[in] value The value of the HTTP request header
	///
	OPENKIT_EXPORT void setOpenKitHttpRequestHeader(struct OpenKitHttpRequest* openKitHttpRequest, const char* name, const char* value);

	/// Pointer to a function that is used to intercept an HTTP request to Dynatrace backend.
	typedef void(*openKitInterceptHttpRequestFunc)(struct OpenKitHttpRequest* /* openKitHttpRequest */);

	/// C wrapper for openkit::IHttpResponse
	struct OpenKitHttpResponse;

	///
	/// Get the original HTTP request URI associated with this HTTP response from the Dynatrace backend.
	///
	/// @remarks Return value is valid within openKitInterceptHttpResponseFunc and must not be freed.
	///
	/// @param[in] openKitHttpResponse OpenKit HTTP response struct
	/// @return Original HTTP request URI associated with this response
	///
	OPENKIT_EXPORT const char* getOpenKitHttpRequestUriFromResponse(const struct OpenKitHttpResponse* openKitHttpResponse);

	///
	/// Get the original HTTP request method associated with this HTTP response from the Dynatrace backend.
	///
	/// @remarks Return value is valid within openKitInterceptHttpResponseFunc and must not be freed.
	///
	/// @param[in] openKitHttpResponse OpenKit HTTP response struct
	/// @return Original HTTP request method associated with this response
	///
	OPENKIT_EXPORT const char* getOpenKitHttpRequestMethodFromResponse(const struct OpenKitHttpResponse* openKitHttpResponse);

	///
	/// Get the HTTP status code of the HTTP response from Dynatrace backend.
	///
	/// @param[in] openKitHttpResponse OpenKit HTTP response struct
	/// @return HTTP status code or a negative value if HTTP response is invalid
	///
	OPENKIT_EXPORT int32_t getOpenKitHttpResponseStatusCode(const struct OpenKitHttpResponse* openKitHttpResponse);

	///
	/// Get the HTTP reason phrase of the HTTP response from Dynatrace backend.
	///
	/// @remarks Return value is valid within openKitInterceptHttpResponseFunc and must not be freed.
	///
	/// @param[in] openKitHttpResponse OpenKit HTTP response struct
	/// @return Reason phrase from HTTP response or @c NULL if response did not contain one
	///
	OPENKIT_EXPORT const char* getOpenKitHttpResponseReasonPhrase(const struct OpenKitHttpResponse* openKitHttpResponse);

	///
	/// Get a list containing all HTTP response header names.
	///
	/// @remarks The returned list must be destroyed using the destroyOpenKitSList function, otherwise memory leaks occur.
	///
	/// @param[in] openKitHttpResponse OpenKit HTTP response struct
	/// @return List containing all response header names
	///
	OPENKIT_EXPORT struct OpenKitSList* getOpenKitHttpResponseHeaderNames(const struct OpenKitHttpResponse* openKitHttpResponse);

	///
	/// Get a boolean indicating whether the HTTP response contains the header with given @c name or not.
	///
	/// @param[in] openKitHttpResponse OpenKit HTTP response struct
	/// @param[in] name The name of the HTTP response header
	/// @return @c true if response header with given @c name is contained, @c false otherwise.
	///
	OPENKIT_EXPORT bool containsOpenKitHttpResponseHeader(const struct OpenKitHttpResponse* openKitHttpResponse, const char* name);

	///
	/// Get the HTTP response header values.
	///
	/// @par
	/// For headers with multiple values, where values are delimited with ","
	/// this method will return a list containing one element.
	/// For instance
	/// @code{.unparsed}
	/// Accept: text/plain, text/html
	/// @endcode
	/// will give @c "text/plain, text/html" as the only entry for "Accept" in the returned list.
	/// If the header occurs multiple times, like the @c Set-Cookie, the list contains
	/// all occurrences in the order they appear in the response.
	///
	/// @remarks The returned list must be destroyed using the destroyOpenKitSList function, otherwise memory leaks occur.
	///
	/// @param[in] openKitHttpResponse OpenKit HTTP response struct
	/// @param[in] name The name of the HTTP response header
	/// @return List containing all values for HTTP response header named @c name or @c NULL if no such HTTP response header exists.
	///
	OPENKIT_EXPORT struct OpenKitSList* getOpenKitHttpResponseHeader(const struct OpenKitHttpResponse* openKitHttpResponse, const char* name);

	/// Pointer to a function that is used to intercept an HTTP response from Dynatrace backend.
	typedef void(*openKitInterceptHttpResponseFunc)(struct OpenKitHttpResponse* /* openKitHttpResponse */);

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
	/// @returns a configuration object that can be used for Dynatrace OpenKit instances
	///
	OPENKIT_EXPORT struct OpenKitConfigurationHandle* createOpenKitConfiguration(const char* endpointURL, const char* applicationID, int64_t deviceID);

	///
	/// Creates an OpenKit configuration object with @c deviceID given as string.
	/// @remarks If the given @c deviceID is longer than 250 characters, only the first 250 characters are used.
	/// @param[in] endpointURL endpoint OpenKit connects to
	/// @param[in] applicationID unique application id
	/// @param[in] deviceID unique device id
	/// @returns a configuration object that can be used for Dynatrace OpenKit instances
	///
	OPENKIT_EXPORT struct OpenKitConfigurationHandle* createOpenKitConfigurationWithStringDeviceID(const char* endpointURL, const char* applicationID, const char* deviceID);

	///
	/// Destroys a given configuration
	/// @param[in] configurationHandle configuration handle to clean up
	///
	OPENKIT_EXPORT void destroyOpenKitConfiguration(struct OpenKitConfigurationHandle* configurationHandle);

	///
	/// Set the log level that is used when using OpenKit's default logger.
	/// @remarks When using a non-default logger, by calling @ref useLoggerForConfiguration, this setting has no effect.
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] defaultLogLevel Log level that is used for OpenKit's default logger.
	///
	OPENKIT_EXPORT void useDefaultLogLevelForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, LOG_LEVEL defaultLogLevel);

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

	///
	/// Set a custom HTTP request interceptor for intercepting OpenKit's HTTP requests to backend.
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] interceptHttpRequestFunc C function pointer that is invoked for each HTTP request to Dynatrace backend.
	///
	OPENKIT_EXPORT void useHttpRequestInterceptorForConfiguration(
		struct OpenKitConfigurationHandle* configurationHandle,
		openKitInterceptHttpRequestFunc interceptHttpRequestFunc
	);

	///
	/// Set a custom HTTP response interceptor for intercepting OpenKit's HTTP responses from backend.
	/// @param[in] configurationHandle configuration storing the given parameter
	/// @param[in] interceptHttpResponseFunc C function pointer that is invoked for each HTTP response from Dynatrace backend.
	///
	OPENKIT_EXPORT void useHttpResponseInterceptorForConfiguration(
		struct OpenKitConfigurationHandle* configurationHandle,
		openKitInterceptHttpResponseFunc interceptHttpResponseFunc
	);

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
	/// Shuts down the OpenKit, ending all open Sessions and waiting for them to be sent.
	/// After calling @c shutdown the openKitHandle is released and must not be used any more.
	/// @param[in] openKitHandle the handle returned by @ref createDynatraceOpenKit
	///
	OPENKIT_EXPORT void shutdownOpenKit(struct OpenKitHandle* openKitHandle);

	///
	/// Waits until OpenKit is fully initialized.
	///
	/// The calling thread is blocked until OpenKit is fully initialized or until OpenKit is shut down using the
	/// shutdown() method.
	///
	/// Be aware, if @ref openkit::DynatraceOpenKitBuilder is wrongly configured, for example when creating an
	/// instance with an incorrect endpoint URL, then this method might hang indefinitely, unless shutdown() is called.
	///
	/// @param[in] openKitHandle the handle returned by @ref createDynatraceOpenKit
	/// @returns @c true when OpenKit is fully initialized, @c false when a shutdown request was made.
	///
	OPENKIT_EXPORT bool waitForInitCompletion(struct OpenKitHandle* openKitHandle);

	///
	/// Waits until OpenKit is fully initialized or the given timeout expired
	///
	/// The calling thread is blocked until OpenKit is fully initialized or until OpenKit is shut down using the
	/// shutdown() method or the timeout expired.
	///
	/// Be aware, if @ref openkit::DynatraceOpenKitBuilder is wrongly configured, for example when creating an
	/// instance with an incorrect endpoint URL, then this method might hang indefinitely, unless shutdown() is called
	/// or timeout expires.
	///
	/// @param[in] openKitHandle the handle returned by @ref createDynatraceOpenKit
	/// @param[in] timeoutMillis The maximum number of milliseconds to wait for initialization being completed.
	/// @returns @c true when OpenKit is fully initialized, @c false when a shutdown request was made or @c timeoutMillis expired.
	///
	OPENKIT_EXPORT bool waitForInitCompletionWithTimeout(struct OpenKitHandle* openKitHandle, int64_t timeoutMillis);

	///
	/// Returns whether OpenKit is initialized or not.
	/// @param[in] openKitHandle the handle returned by @ref createDynatraceOpenKit
	/// @returns @c true if OpenKit is fully initialized, @c false if OpenKit still performs initialization.
	///
	OPENKIT_EXPORT bool isInitialized(struct OpenKitHandle* openKitHandle);


	//--------------
	//  Session
	//--------------

	/// An opaque type that we'll use as a handle
	struct SessionHandle;

	/// Pair that is needed for sending an event
	typedef struct OpenKitPair {
		const char* key;
		const char* value;
	} OpenKitPair;

	///
	/// Creates a session instance which can then be used to create actions.
	/// @param[in] openKitHandle   the handle returned by @ref createDynatraceOpenKit
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
	/// @param[in] openKitHandle   the handle returned by @ref createDynatraceOpenKit
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
	/// this is equivalent to logging off the user.
	/// 
	/// @par
	/// The last non-empty @c userTag is re-applied to split sessions.
	/// Details are described in
	/// https://github.com/Dynatrace/openkit-native/blob/main/docs/internals.md#identify-users-on-split-sessions.
	/// 
	/// @param[in] sessionHandle the handle returned by @ref createSession
	/// @param[in] userTag       id of the user
	///
	OPENKIT_EXPORT void identifyUser(struct SessionHandle* sessionHandle, const char* userTag);

	///
	/// Reports a crash with a specified error name, crash reason and a stacktrace.
	///
	/// @par
	/// If given @c errorName is @c nullptr or an empty string, no crash is reported.
	/// If the @c reason is longer than 1000 characters, it is truncated to this value.
	/// If the @c stacktrace is longer than 128.000 characters, it is truncated according to the last line break.
	///
	/// @param[in] sessionHandle the handle returned by @ref createSession
	/// @param[in] errorName     name of the error leading to the crash (e.g. Exception class)
	/// @param[in] reason        reason or description of that error
	/// @param[in] stacktrace    stacktrace leading to that crash
	///
	OPENKIT_EXPORT void reportCrash(struct SessionHandle* sessionHandle, const char* errorName, const char* reason, const char* stacktrace);

	/// 
	/// Reports a biz event with a mandatory type and additional attributes
	/// 
	/// @param[in] sessionHandle the handle returned by @ref createSession
	/// @param[in] type type of the event which is mandatory
	/// @param[in] attributes additional attributes which are passed along side our internal attributes 
	/// 
	OPENKIT_EXPORT void sendBizEvent(struct SessionHandle* sessionHandle, const char* type, OpenKitPair* attributes, size_t attributesSize);

	/// 
	/// Reports an event with a mandatory type and additional attributes
	/// 
	/// @param[in] sessionHandle the handle returned by @ref createSession
	/// @param[in] name name of the event which is mandatory
	/// @param[in] attributes additional attributes which are passed along side our internal attributes 
	/// 
	OPENKIT_EXPORT void sendEvent(struct SessionHandle* sessionHandle, const char* name, OpenKitPair* attributes, size_t attributesSize);

	//--------------
	//  Root Action
	//--------------

	/// An opaque type that we'll use as a handle
	struct RootActionHandle;

	///
	/// Enters a root action with a specified name in this session.
	/// 
	/// @par
	/// Use leaveRootAction(struct RootActionHandle*) or cancelRootAction(struct RootActionHandle*)
	/// to free allocated resources.
	/// 
	/// @param[in] sessionHandle the handle returned by @ref createSession
	/// @param[in] rootActionName    name of the Action
	/// @returns Root action instance to work with
	///
	OPENKIT_EXPORT struct RootActionHandle* enterRootAction(struct SessionHandle* sessionHandle, const char* rootActionName);

	///
	/// Leaves this root action.
	///
	/// @remarks After calling this method the RootActionHandle* is no longer valid.
	///
	/// @param[in] rootActionHandle the handle returned by @ref enterRootAction
	///
	OPENKIT_EXPORT void leaveRootAction(struct RootActionHandle* rootActionHandle);

	///
	/// Cancels this root action.
	///
	/// @remarks After calling this method the RootActionHandle* is no longer valid.
	///
	/// @par
	/// Canceling a root action is similar to leaveRootAction(struct RootActionHandle*),
	/// except that the data and all unfinished child objects are discarded
	/// instead of being sent.
	///
	/// @param[in] rootActionHandle the handle returned by @ref enterRootAction
	///
	OPENKIT_EXPORT void cancelRootAction(struct RootActionHandle* rootActionHandle);

	///
	/// Gets the duration of this root action in milliseconds.
	///
	/// @par
	/// The duration is defined as current timestamp - start timestamp.
	///
	/// @param[in] rootActionHandle the handle returned by @ref enterRootAction
	/// @return The duration of this root action in milliseconds
	///
	OPENKIT_EXPORT int64_t getDurationOfRootAction(struct RootActionHandle* rootActionHandle);

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
	/// Reports a 32-bit int value with a specified name.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] valueName		name of this value
	/// @param[in] value			value itself
	///
	OPENKIT_EXPORT void reportIntValueOnRootAction(struct RootActionHandle* rootActionHandle, const char* valueName, int32_t value);

	///
	/// Reports a 64-bit int value with a specified name.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] valueName		name of this value
	/// @param[in] value			value itself
	///
	OPENKIT_EXPORT void reportInt64ValueOnRootAction(struct RootActionHandle* rootActionHandle, const char* valueName, int64_t value);

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
	/// Reports an error with a specified name and error code.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] errorName		name of this error
	/// @param[in] errorCode		numeric error code of this error
	///
	OPENKIT_EXPORT void reportErrorCodeOnRootAction(struct RootActionHandle* rootActionHandle, const char* errorName, int32_t errorCode);

	///
	/// Reports an error with a specified name and parameters describing the cause of this error.
	///
	/// @par
	/// If given @c errorName is @c nullptr or an empty string, no error is reported.
	/// If the @c causeDescription is longer than 1000 characters, it is truncated to this value.
	/// If the @c causeStackTrace is longer than 128.000 characters, it is truncated according to the last line break.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] errorName			name of this error
	/// @param[in] causeName			name describing the cause of the error
	/// @param[in] causeDescription		description what caused the eror
	/// @param[in] causeStackTrace		stack trace of the error
	///
	OPENKIT_EXPORT void reportErrorCauseOnRootAction(
		struct RootActionHandle* rootActionHandle,
		const char* errorName,
		const char* causeName,
		const char* causeDescription,
		const char* causeStackTrace
	);

	//--------------
	//  Action
	//--------------

	/// An opaque type that we'll use as a handle
	struct ActionHandle;

	///
	/// Enters an action with a specified name in this root action.
	///
	/// @par
	/// Use leaveAction(struct ActionHandle*) or cancelAction(struct ActionHandle*)
	/// to free allocated resources.
	///
	/// @param[in] rootActionHandle the handle returned by @ref enterRootAction
	/// @param[in] actionName       name of the Action
	/// @returns Root action instance to work with
	///
	OPENKIT_EXPORT struct ActionHandle* enterAction(struct RootActionHandle* rootActionHandle, const char* actionName);

	///
	/// Leaves this action.
	///
	/// @remarks After calling this method the ActionHandle* is no longer valid.
	///
	/// @param[in] actionHandle the handle returned by @ref enterAction
	///
	OPENKIT_EXPORT void leaveAction(struct ActionHandle* actionHandle);

	///
	/// Cancels this action.
	///
	/// @remarks After calling this method the ActionHandle* is no longer valid.
	///
	/// @par
	/// Canceling an action is similar to leaveAction(struct ActionHandle*),
	/// except that the data and all unfinished child objects are discarded
	/// instead of being sent.
	///
	/// @param[in] actionHandle the handle returned by @ref enterAction
	///
	OPENKIT_EXPORT void cancelAction(struct ActionHandle* actionHandle);

	///
	/// Gets the duration of this action in milliseconds.
	///
	/// @par
	/// The duration is defined as current timestamp - start timestamp.
	///
	/// @param[in] actionHandle the handle returned by @ref enterAction
	/// @return The duration of this action in milliseconds
	///
	OPENKIT_EXPORT int64_t getDurationOfAction(struct ActionHandle* actionHandle);

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
	/// Reports a 32-bit int value with a specified name.
	///
	/// @param[in] actionHandle	the handle returned by @ref enterAction
	/// @param[in] valueName	name of this value
	/// @param[in] value		value itself
	///
	OPENKIT_EXPORT void reportIntValueOnAction(struct ActionHandle* actionHandle, const char* valueName, int32_t value);

	///
	/// Reports a 64-bit int value with a specified name.
	///
	/// @param[in] actionHandle	the handle returned by @ref enterAction
	/// @param[in] valueName	name of this value
	/// @param[in] value		value itself
	///
	OPENKIT_EXPORT void reportInt64ValueOnAction(struct ActionHandle* actionHandle, const char* valueName, int64_t value);

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
	/// Reports an error with a specified name and error code.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] errorName		name of this error
	/// @param[in] errorCode		numeric error code of this error
	///
	OPENKIT_EXPORT void reportErrorCodeOnAction(struct ActionHandle* actionHandle, const char* errorName, int32_t errorCode);

	///
	/// Reports an error with a specified name and parameters describing the cause of this error.
	///
	/// @par
	/// If given @c errorName is @c nullptr or an empty string, no error is reported.
	/// If the @c causeDescription is longer than 1000 characters, it is truncated to this value.
	/// If the @c causeStackTrace is longer than 128.000 characters, it is truncated according to the last line break.
	///
	/// @param[in] rootActionHandle	the handle returned by @ref enterRootAction
	/// @param[in] errorName			name of this error
	/// @param[in] causeName			name describing the cause of the error
	/// @param[in] causeDescription		description what caused the eror
	/// @param[in] causeStackTrace		stack trace of the error
	///
	OPENKIT_EXPORT void reportErrorCauseOnAction(
		struct ActionHandle* actionHandle,
		const char* errorName,
		const char* causeName,
		const char* causeDescription,
		const char* causeStackTrace
	);


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
	/// Stops the web request timing with the given respone code. Should be called when the web request is finished.
	/// After calling @c stop the webRequestTracerHandle is released and must not be used any more.
	/// @param[in] webRequestTracerHandle the handle returned by @ref traceWebRequestOnRootAction or @ref traceWebRequestOnAction
	/// @param[in] responseCode response code of this web request
	///
	OPENKIT_EXPORT void stopWebRequestWithResponseCode(struct WebRequestTracerHandle* webRequestTracerhandle, int32_t responseCode);

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
