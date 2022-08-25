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

#include "OpenKit/OpenKit-c.h"
#include "CustomLogger.h"
#include "CustomTrustManager.h"
#include "CustomHttpRequestInterceptor.h"
#include "CustomHttpResponseInterceptor.h"
#include "OpenKitHttpRequest.h"
#include "OpenKitHttpResponse.h"

#include "OpenKit/IOpenKit.h"
#include "OpenKit/DynatraceOpenKitBuilder.h"
#include "OpenKit/AppMonOpenKitBuilder.h"
#include "OpenKit/ISession.h"
#include "OpenKit/IRootAction.h"
#include "OpenKit/IAction.h"
#include "OpenKit/IWebRequestTracer.h"

#include "core/util/DefaultLogger.h"
#include "core/util/StringUtil.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "protocol/ssl/SSLBlindTrustManager.h"
#include "util/json/JsonParser.h"

#include <curl/curl.h>

#include <list>
#include <exception>
#include <assert.h>
#include <string.h>

extern "C" {

	// These macros are for increasing readability only
#define TRY try
#define CATCH_AND_LOG(handle)																			\
	catch (const std::exception& ex)																	\
	{																									\
		if (handle)																						\
		{																								\
			assert((handle)->logger != NULL);															\
			(handle)->logger->error("Exception occurred in %s #%d: %s", __FILE__, __LINE__, ex.what());	\
		}																								\
	}																									\
	catch (...)																							\
	{																									\
		if (handle)																						\
		{																								\
			assert((handle)->logger != NULL);															\
			(handle)->logger->error("Unknown exception occurred in %s #%d", __FILE__, __LINE__);		\
		}																								\
	}

#define CATCH_AND_IGNORE_ALL()																			\
	catch (...)																							\
	{																									\
		/*nop*/																							\
	}

#define FREE_DUPLICATED_STRING(charPointer)																\
	if (charPointer != nullptr)																			\
	{																									\
		free(charPointer);																				\
	}

	char* duplicateString(const char* str)
	{
		char* stringCopy = nullptr;
		if (str != nullptr)
		{
			size_t stringLength = strlen(str);
			if (stringLength > 0)
			{
				stringCopy = (char*)malloc(stringLength + 1);
				if (stringCopy != nullptr)
				{
#if defined(_WIN32) || defined(WIN32)
					strcpy_s(stringCopy, stringLength + 1, str);
#else
					// using strncpy triggers warning in gcc
					// see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88059
					strcpy(stringCopy, str);
#endif
				}
			}
		}

		return stringCopy;
	}

	//--------------
	// TrustManager
	//--------------

	typedef struct TrustManagerHandle
	{
		std::shared_ptr<openkit::ISSLTrustManager> trustManager = nullptr;
	} TrustManagerHandle;

	TrustManagerHandle* createCustomTrustManager(applyTrustManagerFunc applyTrustManagerFunc)
	{
		// Sanity
		TrustManagerHandle* handle = nullptr;
		try
		{
			auto trustManager = std::shared_ptr<openkit::ISSLTrustManager>(new apic::CustomTrustManager(applyTrustManagerFunc));
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new TrustManagerHandle();
			handle->trustManager = trustManager;
		}
		CATCH_AND_IGNORE_ALL()

		return handle;
	}

	static TrustManagerHandle* createStrictTrustManager()
	{
		// Sanity
		TrustManagerHandle* handle = nullptr;
		try
		{
			auto trustManager = std::shared_ptr<openkit::ISSLTrustManager>(new protocol::SSLStrictTrustManager());
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new TrustManagerHandle();
			handle->trustManager = trustManager;
		}
		CATCH_AND_IGNORE_ALL()

		return handle;
	}

	static TrustManagerHandle* createBlindTrustManager()
	{
		// Sanity
		TrustManagerHandle* handle = nullptr;
		try
		{
			auto trustManager = std::shared_ptr<openkit::ISSLTrustManager>(new protocol::SSLBlindTrustManager());
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new TrustManagerHandle();
			handle->trustManager = trustManager;
		}
		CATCH_AND_IGNORE_ALL()

		return handle;
	}

	void destroyTrustManager(TrustManagerHandle* trustManagerHandle)
	{
		// Sanity
		if (trustManagerHandle == nullptr)
		{
			return;
		}

		// release shared pointer
		trustManagerHandle->trustManager = nullptr;
		delete trustManagerHandle;
	}

	//--------------
	//  Logger
	//--------------

	typedef struct LoggerHandle
	{
		std::shared_ptr<openkit::ILogger> logger = nullptr;
	} LoggerHandle;

	LoggerHandle* createLogger(levelEnabledFunc levelEnabledFunc, logFunc logFunc)
	{
		// Sanity
		if (levelEnabledFunc == nullptr || logFunc == nullptr)
		{
			return nullptr;
		}

		LoggerHandle* handle = nullptr;
		try
		{
			auto logger = std::make_shared<apic::CustomLogger>(levelEnabledFunc, logFunc);
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new LoggerHandle();
			handle->logger = logger;
		}
		CATCH_AND_IGNORE_ALL()

		return handle;
	}

	static openkit::LogLevel toCppLogLevel(LOG_LEVEL cLogLevel)
	{
		switch (cLogLevel)
		{
		case LOGLEVEL_DEBUG:
			return openkit::LogLevel::LOG_LEVEL_DEBUG;
		case LOGLEVEL_INFO:
			return openkit::LogLevel::LOG_LEVEL_INFO;
		case LOGLEVEL_WARN:
			return openkit::LogLevel::LOG_LEVEL_WARN;
		case LOGLEVEL_ERROR:
			return openkit::LogLevel::LOG_LEVEL_ERROR;
		default:
			return openkit::LogLevel::LOG_LEVEL_WARN;
		}
	}

	static LoggerHandle* createDefaultLogger(LOG_LEVEL logLevel)
	{
		LoggerHandle* handle = nullptr;
		try
		{
			auto logger = std::make_shared<core::util::DefaultLogger>(toCppLogLevel(logLevel));
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new LoggerHandle();
			handle->logger = logger;
		}
		CATCH_AND_IGNORE_ALL()

		return handle;
	}

	void destroyLogger(LoggerHandle* loggerHandle)
	{
		// Sanity
		if (loggerHandle == nullptr)
		{
			return;
		}

		// release shared pointer
		loggerHandle->logger = nullptr;
		delete loggerHandle;
	}

	static struct OpenKitSList* createNode(const std::string& value)
	{
		struct OpenKitSList* node;
		try
		{
			// might throw std::bad_alloc
			node = new OpenKitSList();
		}
		catch (...)
		{
			return nullptr;
		}
		node->next = nullptr;
		node->value = duplicateString(value.c_str());

		if (node->value == nullptr)
		{
			delete node;
			node = nullptr;
		}

		return node;
	}
	
	static struct OpenKitSList* createFromCxxList(const std::list<std::string>& cxxList)
	{
		struct OpenKitSList* first = nullptr;
		struct OpenKitSList* current = nullptr;

		for (const auto& entry : cxxList)
		{
			struct OpenKitSList* tmp = createNode(entry);
			if (tmp == nullptr)
			{
				// destroy all nodes that have been created so far
				destroyOpenKitSList(first);
				first = nullptr;
				current = nullptr;

				break;
			}

			if (first == nullptr)
			{
				// first list node
				first = tmp;
				current = tmp;
			}
			else
			{
				current->next = tmp;
				current = tmp;
			}
		}

		return first;
	}

	void destroyOpenKitSList(struct OpenKitSList* sList)
	{
		struct OpenKitSList* current = sList;
		while (current != nullptr)
		{
			struct OpenKitSList* tmp = current->next;
			
			FREE_DUPLICATED_STRING(const_cast<char*>(current->value));
			current->value = nullptr;
			current->next = nullptr;

			delete current;

			current = tmp;
		}
	}

	//--------------------------------------
	//  HTTP request & response interception
	//--------------------------------------

	const char* getOpenKitHttpRequestUri(const struct OpenKitHttpRequest* openKitHttpRequest)
	{
		if (openKitHttpRequest == nullptr || openKitHttpRequest->mHttpRequest == nullptr)
		{
			return nullptr;
		}

		TRY
		{
			return openKitHttpRequest->mHttpRequest->getUri().c_str();
		}
		CATCH_AND_IGNORE_ALL()

		return nullptr;
	}

	const char* getOpenKitHttpRequestMethod(const struct OpenKitHttpRequest* openKitHttpRequest)
	{
		if (openKitHttpRequest == nullptr || openKitHttpRequest->mHttpRequest == nullptr)
		{
			return nullptr;
		}

		TRY
		{
			return openKitHttpRequest->mHttpRequest->getMethod().c_str();
		}
		CATCH_AND_IGNORE_ALL()

		return nullptr;
	}

	struct OpenKitSList* getOpenKitHttpRequestHeaderNames(const struct OpenKitHttpRequest* openKitHttpRequest)
	{
		if (openKitHttpRequest == nullptr || openKitHttpRequest->mHttpRequest == nullptr)
		{
			return nullptr;
		}

		try
		{
			return createFromCxxList(openKitHttpRequest->mHttpRequest->getHeaderNames());
		}
		CATCH_AND_IGNORE_ALL()

		return nullptr;
	}

	bool containsOpenKitHttpRequestHeader(const struct OpenKitHttpRequest* openKitHttpRequest, const char* name)
	{
		if (openKitHttpRequest == nullptr || openKitHttpRequest->mHttpRequest == nullptr)
		{
			return false;
		}

		TRY
		{
			return openKitHttpRequest->mHttpRequest->containsHeader(name);
		}
		CATCH_AND_IGNORE_ALL()

		return false;
	}

	const char* getOpenKitHttpRequestHeader(const struct OpenKitHttpRequest* openKitHttpRequest, const char* name)
	{
		if (openKitHttpRequest == nullptr || openKitHttpRequest->mHttpRequest == nullptr)
		{
			return nullptr;
		}

		TRY
		{
			return openKitHttpRequest->mHttpRequest->getHeader(name).c_str();
		}
		CATCH_AND_IGNORE_ALL()

		return nullptr;
	}

	void setOpenKitHttpRequestHeader(struct OpenKitHttpRequest* openKitHttpRequest, const char* name, const char* value)
	{
		if (openKitHttpRequest == nullptr || openKitHttpRequest->mHttpRequest == nullptr)
		{
			return;
		}

		TRY
		{
			openKitHttpRequest->mHttpRequest->setHeader(name, value);
		}
		CATCH_AND_IGNORE_ALL()
	}

	const char* getOpenKitHttpRequestUriFromResponse(const struct OpenKitHttpResponse* openKitHttpResponse)
	{
		if (openKitHttpResponse == nullptr || openKitHttpResponse->mHttpResponse == nullptr)
		{
			return nullptr;
		}

		TRY
		{
			return openKitHttpResponse->mHttpResponse->getRequestUri().c_str();
		}
		CATCH_AND_IGNORE_ALL()

		return nullptr;
	}

	const char* getOpenKitHttpRequestMethodFromResponse(const struct OpenKitHttpResponse* openKitHttpResponse)
	{
		if (openKitHttpResponse == nullptr || openKitHttpResponse->mHttpResponse == nullptr)
		{
			return nullptr;
		}

		TRY
		{
			return openKitHttpResponse->mHttpResponse->getRequestMethod().c_str();
		}
		CATCH_AND_IGNORE_ALL()

		return nullptr;
	}

	int32_t getOpenKitHttpResponseStatusCode(const struct OpenKitHttpResponse* openKitHttpResponse)
	{
		if (openKitHttpResponse == nullptr || openKitHttpResponse->mHttpResponse == nullptr)
		{
			return -1;
		}

		TRY
		{
			return openKitHttpResponse->mHttpResponse->getStatusCode();
		}
		CATCH_AND_IGNORE_ALL()

		return -1;
	}

	const char* getOpenKitHttpResponseReasonPhrase(const struct OpenKitHttpResponse* openKitHttpResponse)
	{
		if (openKitHttpResponse == nullptr || openKitHttpResponse->mHttpResponse == nullptr)
		{
			return nullptr;
		}

		TRY
		{
			return openKitHttpResponse->mHttpResponse->getReasonPhrase().c_str();
		}
		CATCH_AND_IGNORE_ALL()

		return nullptr;
	}

	struct OpenKitSList* getOpenKitHttpResponseHeaderNames(const struct OpenKitHttpResponse* openKitHttpResponse)
	{
		if (openKitHttpResponse == nullptr || openKitHttpResponse->mHttpResponse == nullptr)
		{
			return nullptr;
		}

		TRY
		{
			return createFromCxxList(openKitHttpResponse->mHttpResponse->getHeaderNames());
		}
		CATCH_AND_IGNORE_ALL()

		return nullptr;
	}

	bool containsOpenKitHttpResponseHeader(const struct OpenKitHttpResponse* openKitHttpResponse, const char* name)
	{
		if (openKitHttpResponse == nullptr || openKitHttpResponse->mHttpResponse == nullptr)
		{
			return false;
		}

		TRY
		{
			return openKitHttpResponse->mHttpResponse->containsHeader(name);
		}
		CATCH_AND_IGNORE_ALL()

		return false;
	}

	struct OpenKitSList* getOpenKitHttpResponseHeader(const struct OpenKitHttpResponse* openKitHttpResponse, const char* name)
	{
		if (openKitHttpResponse == nullptr || openKitHttpResponse->mHttpResponse == nullptr)
		{
			return nullptr;
		}

		TRY
		{
			std::string headerName = name;

			// return header values only if header is known
			// otherwise fallback to returning nullptr at end of function
			if (openKitHttpResponse->mHttpResponse->containsHeader(headerName))
			{
				return createFromCxxList(openKitHttpResponse->mHttpResponse->getHeader(headerName));
			}
		}
		CATCH_AND_IGNORE_ALL()

		return nullptr;
	}


	//-----------------------
	//  OpenKit configuration
	//-----------------------

	typedef struct OpenKitConfigurationHandle
	{
		char* endpointURL = nullptr;
		char* applicationID = nullptr;
		int64_t deviceID = -1;
		char* origDeviceID = nullptr;
		char* applicationName = nullptr;
		LoggerHandle* loggerHandle = nullptr;
		LOG_LEVEL defaultLogLevel = LOGLEVEL_WARN;
		bool ownsLoggerHandle = false;
		char* applicationVersion = nullptr;
		TRUST_MODE trustMode = STRICT_TRUST;
		TrustManagerHandle* trustManagerHandle = nullptr;
		bool ownsTrustManagerHandle = false;
		char* operatingSystem = nullptr;
		char* manufacturer = nullptr;
		char* modelID = nullptr;
		int64_t beaconCacheMaxRecordAge = -1;
		int64_t beaconCacheLowerMemoryBoundary = -1;
		int64_t beaconCacheUpperMemoryBoundary = -1;
		DataCollectionLevel dataCollectionLevel = DATA_COLLECTION_LEVEL_USER_BEHAVIOR;
		CrashReportingLevel crashReportingLevel = CRASH_REPORTING_LEVEL_OPT_IN_CRASHES;
		openKitInterceptHttpRequestFunc interceptHttpRequestFunc = nullptr;
		openKitInterceptHttpResponseFunc interceptHttpResponseFunc = nullptr;
	} OpenKitConfigurationHandle;

	struct OpenKitConfigurationHandle* createOpenKitConfigurationWithOrigAndHashedDeviceId(const char* endpointURL, const char* applicationID, int64_t deviceID, const char* origDeviceID)
	{
		OpenKitConfigurationHandle* handle = nullptr;
		try
		{
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new OpenKitConfigurationHandle();
			if (handle != nullptr)
			{
				handle->endpointURL = duplicateString(endpointURL);
				handle->applicationID = duplicateString(applicationID);
				handle->deviceID = deviceID;
				handle->origDeviceID = duplicateString(origDeviceID);
			}
		}
		CATCH_AND_IGNORE_ALL()

		return handle;
	}

	struct OpenKitConfigurationHandle* createOpenKitConfiguration(const char* endpointURL, const char* applicationID, int64_t deviceID)
	{
		return createOpenKitConfigurationWithOrigAndHashedDeviceId(endpointURL, applicationID, deviceID, core::util::StringUtil::toInvariantString(deviceID).c_str());
	}

	struct OpenKitConfigurationHandle* createOpenKitConfigurationWithStringDeviceID(const char* endpointURL, const char* applicationID, const char* deviceID)
	{
		return createOpenKitConfigurationWithOrigAndHashedDeviceId(endpointURL, applicationID, core::util::StringUtil::toNumericOr64BitHash(deviceID), deviceID);
	}


	void destroyOpenKitConfiguration(struct OpenKitConfigurationHandle* configurationHandle)
	{
		// Sanity
		if (configurationHandle == nullptr)
		{
			return;
		}

		//clear string copies allocated with strdup
		FREE_DUPLICATED_STRING(configurationHandle->endpointURL);
		FREE_DUPLICATED_STRING(configurationHandle->applicationID);
		FREE_DUPLICATED_STRING(configurationHandle->applicationName);
		FREE_DUPLICATED_STRING(configurationHandle->applicationVersion);
		FREE_DUPLICATED_STRING(configurationHandle->operatingSystem);
		FREE_DUPLICATED_STRING(configurationHandle->manufacturer);
		FREE_DUPLICATED_STRING(configurationHandle->modelID);

		// release configuration object
		delete configurationHandle;
		configurationHandle = nullptr;
	}

	void useDefaultLogLevelForConfiguration(OpenKitConfigurationHandle* configurationHandle, LOG_LEVEL defaultLogLevel)
	{
		if (configurationHandle != nullptr)
		{
			configurationHandle->defaultLogLevel = defaultLogLevel;
		}
	}

	void useLoggerForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, struct LoggerHandle* loggerHandle)
	{
		if (configurationHandle != nullptr)
		{
			configurationHandle->loggerHandle = loggerHandle;
		}
	}

	void useApplicationVersionForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* applicationVersion)
	{
		//sanity
		if (configurationHandle != nullptr && applicationVersion != nullptr)
		{
			configurationHandle->applicationVersion = duplicateString(applicationVersion);
		}
	}

	void useApplicationNameForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* applicationName)
	{
		//sanity
		if (configurationHandle != nullptr && applicationName != nullptr)
		{
			configurationHandle->applicationName = duplicateString(applicationName);
		}
	}

	void useTrustModeForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, TRUST_MODE trustMode, struct TrustManagerHandle* trustManagerHandle)
	{
		//sanity
		if (configurationHandle != nullptr)
		{
			configurationHandle->trustMode = trustMode;
			configurationHandle->trustManagerHandle = trustManagerHandle;
		}
	}

	void useOperatingSystemForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* operatingSystem)
	{
		//sanity
		if (configurationHandle != nullptr && operatingSystem != nullptr)
		{
			configurationHandle->operatingSystem = duplicateString(operatingSystem);
		}
	}

	void useManufacturerForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* manufacturer)
	{
		//sanity
		if (configurationHandle != nullptr && manufacturer != nullptr)
		{
			configurationHandle->manufacturer = duplicateString(manufacturer);
		}
	}

	void useModelIDForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* modelID)
	{
		//sanity
		if (configurationHandle != nullptr && modelID != nullptr)
		{
			configurationHandle->modelID = duplicateString(modelID);
		}
	}

	void useBeaconCacheBehaviorForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, int64_t beaconCacheMaxRecordAge, int64_t beaconCacheLowerMemoryBoundary, int64_t beaconCacheUpperMemoryBoundary)
	{
		//sanity
		if (configurationHandle != nullptr && beaconCacheMaxRecordAge >= 0)
		{
			configurationHandle->beaconCacheMaxRecordAge = beaconCacheMaxRecordAge;
		}
		if (configurationHandle != nullptr && beaconCacheLowerMemoryBoundary >= 0)
		{
			configurationHandle->beaconCacheLowerMemoryBoundary = beaconCacheLowerMemoryBoundary;
		}
		if (configurationHandle != nullptr && beaconCacheUpperMemoryBoundary >= 0)
		{
			configurationHandle->beaconCacheUpperMemoryBoundary = beaconCacheUpperMemoryBoundary;
		}
	}

	void useDataCollectionLevelForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, DataCollectionLevel dataCollectionLevel)
	{
		if (configurationHandle != nullptr)
		{
			configurationHandle->dataCollectionLevel = dataCollectionLevel;
		}
	}

	void useCrashReportingLevelForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, CrashReportingLevel crashReportingLevel)
	{
		if (configurationHandle != nullptr)
		{
			configurationHandle->crashReportingLevel = crashReportingLevel;
		}
	}

	void useHttpRequestInterceptorForConfiguration(
		struct OpenKitConfigurationHandle* configurationHandle,
		openKitInterceptHttpRequestFunc interceptHttpRequestFunc
	)
	{
		if (configurationHandle != nullptr && interceptHttpRequestFunc != nullptr)
		{
			configurationHandle->interceptHttpRequestFunc = interceptHttpRequestFunc;
		}
	}

	void useHttpResponseInterceptorForConfiguration(
		struct OpenKitConfigurationHandle* configurationHandle,
		openKitInterceptHttpResponseFunc interceptHttpResponseFunc
	)
	{
		if (configurationHandle != nullptr && interceptHttpResponseFunc != nullptr)
		{
			configurationHandle->interceptHttpResponseFunc = interceptHttpResponseFunc;
		}
	}

	//--------------
	//  OpenKit
	//--------------

	typedef struct OpenKitHandle
	{
		std::shared_ptr<openkit::IOpenKit> sharedPointer = nullptr;
		std::shared_ptr<openkit::ILogger> logger = nullptr;
		LoggerHandle* loggerHandle = nullptr;
		bool ownsLoggerHandle = false;
		TrustManagerHandle* trustManagerHandle = nullptr;
		bool ownsTrustManagerHandle = false;
	} OpenKitHandle;

	static TrustManagerHandle* createTrustManagerHandle(LoggerHandle* loggerHandle, TRUST_MODE trustMode, TrustManagerHandle* trustManagerHandle)
	{
		if (trustMode == TRUST_MODE::BLIND_TRUST)
		{
			return createBlindTrustManager();
		}
		else if (trustMode == TRUST_MODE::CUSTOM_TRUST && trustManagerHandle != nullptr)
		{
			return trustManagerHandle;
		}
		else
		{
			// default is "strict"
			if (loggerHandle && trustMode != TRUST_MODE::STRICT_TRUST)
			{
				loggerHandle->logger->warning("Unexpected trust mode %d. Falling back to STRICT trust mode", trustMode);
			}
			return createStrictTrustManager();
		}
	}

	static LoggerHandle* getLoggerHandle(OpenKitConfigurationHandle* configurationHandle)
	{
		if (configurationHandle->loggerHandle == nullptr)
		{
			// caller did not provide a logger handle -> fallback to NullLogger
			struct LoggerHandle* defaultLogger = createDefaultLogger(configurationHandle->defaultLogLevel);

			configurationHandle->loggerHandle = defaultLogger;
			configurationHandle->ownsLoggerHandle = true;
		}

		return configurationHandle->loggerHandle;
	}

	static void initializeOpenKitBuilder(openkit::AbstractOpenKitBuilder& builder, struct OpenKitConfigurationHandle* configurationHandle)
	{
		// create trust manager handle based on given TRUST_MODE & existing TrustManagerHandle
		// the result might either be a completly newly created TrustManagerHandle* or
		// a pointer pointing to the same struct as given trustManagerHandle.
		// In case the ptr is newly created the shutdownOpenKit is responsible for destroying it.
		TrustManagerHandle* usedTrustManagerHandle = createTrustManagerHandle(configurationHandle->loggerHandle, configurationHandle->trustMode, configurationHandle->trustManagerHandle);
		configurationHandle->ownsTrustManagerHandle = usedTrustManagerHandle != configurationHandle->trustManagerHandle;
		configurationHandle->trustManagerHandle = usedTrustManagerHandle;

		LoggerHandle* loggerHandle = getLoggerHandle(configurationHandle);
		if (loggerHandle != nullptr)
		{
			builder.withLogger(loggerHandle->logger);
		}

		if (configurationHandle->trustManagerHandle != nullptr)
		{
			builder.withTrustManager(usedTrustManagerHandle->trustManager);
		}

		if (configurationHandle->applicationVersion != nullptr)
		{
			builder.withApplicationVersion(configurationHandle->applicationVersion);
		}

		if (configurationHandle->operatingSystem != nullptr)
		{
			builder.withOperatingSystem(configurationHandle->operatingSystem);
		}

		if (configurationHandle->manufacturer != nullptr)
		{
			builder.withManufacturer(configurationHandle->manufacturer);
		}

		if (configurationHandle->modelID != nullptr)
		{
			builder.withModelID(configurationHandle->modelID);
		}

		if (configurationHandle->beaconCacheMaxRecordAge >= 0)
		{
			builder.withBeaconCacheMaxRecordAge(configurationHandle->beaconCacheMaxRecordAge);
		}

		if (configurationHandle->beaconCacheLowerMemoryBoundary >= 0)
		{
			builder.withBeaconCacheLowerMemoryBoundary(configurationHandle->beaconCacheLowerMemoryBoundary);
		}

		if (configurationHandle->beaconCacheUpperMemoryBoundary >= 0)
		{
			builder.withBeaconCacheUpperMemoryBoundary(configurationHandle->beaconCacheUpperMemoryBoundary);
		}

		if (configurationHandle->dataCollectionLevel < DATA_COLLECTION_LEVEL_COUNT)
		{
			builder.withDataCollectionLevel((openkit::DataCollectionLevel)configurationHandle->dataCollectionLevel);
		}

		if (configurationHandle->crashReportingLevel < CRASH_REPORTING_LEVEL_COUNT)
		{
			builder.withCrashReportingLevel((openkit::CrashReportingLevel)configurationHandle->crashReportingLevel);
		}

		if (configurationHandle->interceptHttpRequestFunc != nullptr)
		{
			builder.withHttpRequestInterceptor(
				std::make_shared<apic::CustomHttpRequestInterceptor>(configurationHandle->interceptHttpRequestFunc));
		}

		if (configurationHandle->interceptHttpResponseFunc != nullptr)
		{
			builder.withHttpResponseInterceptor(
				std::make_shared<apic::CustomHttpResponseInterceptor>(configurationHandle->interceptHttpResponseFunc));
		}
	}

	static OpenKitHandle* createOpenKitHandle(struct OpenKitConfigurationHandle* configurationHandle, std::shared_ptr<openkit::IOpenKit> openKit)
	{
		// storing the returned shared pointer in the handle prevents it from going out of scope
		OpenKitHandle* handle = nullptr;
		TRY
		{
			handle = new OpenKitHandle();
			handle->sharedPointer = openKit;
			handle->logger = configurationHandle->loggerHandle->logger;
			handle->loggerHandle = configurationHandle->loggerHandle;
			handle->ownsLoggerHandle = configurationHandle->ownsLoggerHandle;
			handle->trustManagerHandle = configurationHandle->trustManagerHandle;
			handle->ownsTrustManagerHandle = configurationHandle->ownsTrustManagerHandle;
		}
		CATCH_AND_LOG(configurationHandle->loggerHandle)

		return handle;
	}

	OpenKitHandle* createDynatraceOpenKit(struct OpenKitConfigurationHandle* configurationHandle)
	{
		OpenKitHandle* handle = nullptr;
		TRY
		{
			openkit::DynatraceOpenKitBuilder builder(configurationHandle->endpointURL, configurationHandle->applicationID, configurationHandle->origDeviceID);

			// initialize the abstract builder
			initializeOpenKitBuilder(builder, configurationHandle);

			return createOpenKitHandle(configurationHandle, builder.build());
		}
		CATCH_AND_LOG(configurationHandle->loggerHandle)

		return handle;
	}

	OpenKitHandle* createAppMonOpenKit(struct OpenKitConfigurationHandle* configurationHandle)
	{
		OpenKitHandle* handle = nullptr;
		TRY
		{
			openkit::AppMonOpenKitBuilder builder(configurationHandle->endpointURL, configurationHandle->applicationName, configurationHandle->origDeviceID);

			// initialize the abstract builder
			initializeOpenKitBuilder(builder, configurationHandle);

			return createOpenKitHandle(configurationHandle, builder.build());
		}
		CATCH_AND_LOG(configurationHandle->loggerHandle)

		return handle;
	}

	void shutdownOpenKit(OpenKitHandle* openKitHandle)
	{
		// Sanity
		if (openKitHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the OpenKit instance from the handle and call the respective method
			assert(openKitHandle->sharedPointer != nullptr);
			openKitHandle->sharedPointer->shutdown();

			// release shared pointer
			openKitHandle->sharedPointer = nullptr;
			openKitHandle->logger = nullptr;
			if (openKitHandle->ownsTrustManagerHandle &&  openKitHandle->trustManagerHandle != nullptr)
			{
				destroyTrustManager(openKitHandle->trustManagerHandle);
				openKitHandle->trustManagerHandle = nullptr;
			}
			if (openKitHandle->ownsLoggerHandle && openKitHandle->loggerHandle != nullptr)
			{
				destroyLogger(openKitHandle->loggerHandle);
				openKitHandle->loggerHandle = nullptr;
			}

			delete openKitHandle;
		}
		CATCH_AND_LOG(openKitHandle)
	}

	bool waitForInitCompletion(struct OpenKitHandle* openKitHandle)
	{
		TRY
		{
			if (openKitHandle)
			{
				// retrieve the OpenKit instance from the handle and call the respective method
				assert(openKitHandle->sharedPointer != nullptr);
				return openKitHandle->sharedPointer->waitForInitCompletion();
			}
		}
		CATCH_AND_LOG(openKitHandle)

		return false;
	}

	bool waitForInitCompletionWithTimeout(struct OpenKitHandle* openKitHandle, int64_t timeoutMillis)
	{
		TRY
		{
			if (openKitHandle)
			{
				// retrieve the OpenKit instance from the handle and call the respective method
				assert(openKitHandle->sharedPointer != nullptr);
				openKitHandle->sharedPointer->waitForInitCompletion(timeoutMillis);
			}
		}
		CATCH_AND_LOG(openKitHandle)

		return false;
	}

	bool isInitialized(struct OpenKitHandle* openKitHandle)
	{
		TRY
		{
			if (openKitHandle)
			{
				// retrieve the OpenKit instance from the handle and call the respective method
				assert(openKitHandle->sharedPointer != nullptr);
				return openKitHandle->sharedPointer->isInitialized();
			}
		}
		CATCH_AND_LOG(openKitHandle)

		return false;
	}

	//--------------
	//  Session
	//--------------

	typedef struct SessionHandle
	{
		std::shared_ptr<openkit::ISession> sharedPointer = nullptr;
		std::shared_ptr<openkit::ILogger> logger = nullptr;
	} SessionHandle;

	SessionHandle* createSession(OpenKitHandle* openKitHandle, const char* clientIPAddress)
	{
		// Sanity
		if (openKitHandle == nullptr)
		{
			return nullptr;
		}

		SessionHandle* handle = nullptr;
		TRY
		{
			// retrieve the OpenKit instance from the handle and call the respective method
			assert(openKitHandle->sharedPointer != nullptr);
			std::shared_ptr<openkit::ISession> session = openKitHandle->sharedPointer->createSession(clientIPAddress);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new SessionHandle();
			handle->sharedPointer = session;
			handle->logger = openKitHandle->logger;
		}
		CATCH_AND_LOG(openKitHandle)

		return handle;
	}

	SessionHandle* createSessionWithAutoIpDetermination(struct OpenKitHandle* openKitHandle)
	{
		return createSession(openKitHandle, nullptr);
	}

	void endSession(SessionHandle* sessionHandle)
	{
		// Sanity
		if (sessionHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the Session instance from the handle and call the respective method
			assert(sessionHandle->sharedPointer != nullptr);
			sessionHandle->sharedPointer->end();

			// release shared pointer
			sessionHandle->sharedPointer = nullptr;
			sessionHandle->logger = nullptr;
			delete sessionHandle;
		}
		CATCH_AND_LOG(sessionHandle)
	}

	void identifyUser(SessionHandle* sessionHandle, const char* userTag)
	{
		TRY
		{
			if (sessionHandle)
			{
				// retrieve the Session instance from the handle and call the respective method
				assert(sessionHandle->sharedPointer != nullptr);
				sessionHandle->sharedPointer->identifyUser(userTag);
			}
		}
		CATCH_AND_LOG(sessionHandle)
	}

	void reportCrash(SessionHandle* sessionHandle, const char* errorName, const char* reason, const char* stacktrace)
	{
		TRY
		{
			if (sessionHandle)
			{
				// retrieve the Session instance from the handle and call the respective method
				assert(sessionHandle->sharedPointer != nullptr);
				sessionHandle->sharedPointer->reportCrash(errorName, reason, stacktrace);
			}
		}
		CATCH_AND_LOG(sessionHandle)
	}

	void sendBizEvent(SessionHandle* sessionHandle, const char* type, struct Pair* attributes, size_t attributesSize)
	{
		TRY
		{
			if (sessionHandle)
			{
				// retrieve the Session instance from the handle and call the respective method
				assert(sessionHandle->sharedPointer != nullptr);

				auto convertedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

				if (attributes != nullptr)
				{
					for (size_t i = 0; i < attributesSize; i++)
					{
						util::json::JsonParser jsonParser(attributes[i].value);
						convertedMap->insert(std::make_pair(attributes[i].key, jsonParser.parse()));
					}
				}

				sessionHandle->sharedPointer->sendBizEvent(type, convertedMap);
			}
		}
		CATCH_AND_LOG(sessionHandle)
	}

	void sendEvent(SessionHandle* sessionHandle, const char* name, struct Pair* attributes, size_t attributesSize)
	{
		TRY
		{
			if (sessionHandle)
			{
				// retrieve the Session instance from the handle and call the respective method
				assert(sessionHandle->sharedPointer != nullptr);

				auto convertedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

				if (attributes != nullptr)
				{
					for (size_t i = 0; i < attributesSize; i++)
					{
						util::json::JsonParser jsonParser(attributes[i].value);
						convertedMap->insert(std::make_pair(attributes[i].key, jsonParser.parse()));
					}
				}

				sessionHandle->sharedPointer->sendEvent(name, convertedMap);
			}
		}
		CATCH_AND_LOG(sessionHandle)
	}

	//--------------
	//  Root Action
	//--------------

	typedef struct RootActionHandle
	{
		std::shared_ptr<openkit::IRootAction> sharedPointer = nullptr;
		std::shared_ptr<openkit::ILogger> logger = nullptr;
	} RootActionHandle;

	RootActionHandle* enterRootAction(SessionHandle* sessionHandle, const char* rootActionName)
	{
		// Sanity
		if (sessionHandle == nullptr)
		{
			return nullptr;
		}

		RootActionHandle* handle = nullptr;
		TRY
		{
			// retrieve the Session instance from the handle and call the respective method
			assert(sessionHandle->sharedPointer != nullptr);
			std::shared_ptr<openkit::IRootAction> rootAction = sessionHandle->sharedPointer->enterAction(rootActionName);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new RootActionHandle();
			handle->sharedPointer = rootAction;
			handle->logger = sessionHandle->logger;
		}
		CATCH_AND_LOG(sessionHandle)

		return handle;
	}

	static void releaseRootAction(RootActionHandle* rootActionHandle)
	{
		rootActionHandle->sharedPointer = nullptr;
		rootActionHandle->logger = nullptr;
		delete rootActionHandle;
	}

	void leaveRootAction(RootActionHandle* rootActionHandle)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(rootActionHandle->sharedPointer != nullptr);
			rootActionHandle->sharedPointer->leaveAction();

			// cleanup
			releaseRootAction(rootActionHandle);
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void cancelRootAction(struct RootActionHandle* rootActionHandle)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(rootActionHandle->sharedPointer != nullptr);
			rootActionHandle->sharedPointer->cancelAction();

			// cleanup
			releaseRootAction(rootActionHandle);
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	int64_t getDurationOfRootAction(struct RootActionHandle* rootActionHandle)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				return rootActionHandle->sharedPointer->getDuration().count();
			}
		}
		CATCH_AND_LOG(rootActionHandle)

		return -1;
	}

	void reportEventOnRootAction(RootActionHandle* rootActionHandle, const char* eventName)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportEvent(eventName);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportIntValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, int32_t value)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportInt64ValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, int64_t value)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportDoubleValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, double value)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportStringValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, const char* value)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportErrorOnRootAction(RootActionHandle* rootActionHandle, const char* errorName, int32_t errorCode, const char* /*reason*/)
	{
		reportErrorCodeOnRootAction(rootActionHandle, errorName, errorCode);
	}

	void reportErrorCodeOnRootAction(RootActionHandle* rootActionHandle, const char* errorName, int32_t errorCode)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportError(errorName, errorCode);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportErrorCauseOnRootAction(
		struct RootActionHandle* rootActionHandle,
		const char* errorName,
		const char* causeName,
		const char* causeDescription,
		const char* causeStackTrace
	)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportError(errorName, causeName, causeDescription, causeStackTrace);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}


	//--------------
	//  Action
	//--------------

	typedef struct ActionHandle
	{
		std::shared_ptr<openkit::IAction> sharedPointer = nullptr;
		std::shared_ptr<openkit::ILogger> logger = nullptr;
	} ActionHandle;

	ActionHandle* enterAction(RootActionHandle* rootActionHandle, const char* actionName)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return nullptr;
		}

		ActionHandle* handle = nullptr;
		TRY
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(rootActionHandle->sharedPointer != nullptr);
			std::shared_ptr<openkit::IAction> action = rootActionHandle->sharedPointer->enterAction(actionName);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new ActionHandle();
			handle->sharedPointer = action;
			handle->logger = rootActionHandle->logger;
		}
		CATCH_AND_LOG(rootActionHandle)

		return handle;
	}

	static void releaseAction(ActionHandle* actionHandle)
	{
		// release shared pointer
		actionHandle->sharedPointer = nullptr;
		actionHandle->logger = nullptr;
		delete actionHandle;
	}

	void leaveAction(ActionHandle* actionHandle)
	{
		// Sanity
		if (actionHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the Action instance from the handle and call the respective method
			assert(actionHandle->sharedPointer != nullptr);
			actionHandle->sharedPointer->leaveAction();

			// cleanup
			releaseAction(actionHandle);
		}
		CATCH_AND_LOG(actionHandle)
	}

	void cancelAction(struct ActionHandle* actionHandle)
	{
		// Sanity
		if (actionHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the Action instance from the handle and call the respective method
			assert(actionHandle->sharedPointer != nullptr);
			actionHandle->sharedPointer->cancelAction();

			// cleanup
			releaseAction(actionHandle);
		}
		CATCH_AND_LOG(actionHandle)
	}

	int64_t getDurationOfAction(struct ActionHandle* actionHandle)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				return actionHandle->sharedPointer->getDuration().count();
			}
		}
		CATCH_AND_LOG(actionHandle)

		return -1;
	}

	void reportEventOnAction(ActionHandle* actionHandle, const char* eventName)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportEvent(eventName);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}

	void reportIntValueOnAction(ActionHandle* actionHandle, const char* valueName, int32_t value)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}

	void reportInt64ValueOnAction(ActionHandle* actionHandle, const char* valueName, int64_t value)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}

	void reportDoubleValueOnAction(ActionHandle* actionHandle, const char* valueName, double value)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}

	void reportStringValueOnAction(ActionHandle* actionHandle, const char* valueName, const char* value)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}

	void reportErrorOnAction(ActionHandle* actionHandle, const char* errorName, int32_t errorCode, const char* /*reason*/)
	{
		reportErrorCodeOnAction(actionHandle, errorName, errorCode);
	}

	void reportErrorCodeOnAction(ActionHandle* actionHandle, const char* errorName, int32_t errorCode)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportError(errorName, errorCode);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}

	void reportErrorCauseOnAction(
		struct ActionHandle* actionHandle,
		const char* errorName,
		const char* causeName,
		const char* causeDescription,
		const char* causeStackTrace
	)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportError(errorName, causeName, causeDescription, causeStackTrace);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}


	//--------------------
	//  Webrequest Tracer
	//--------------------

	typedef struct WebRequestTracerHandle
	{
		std::shared_ptr<openkit::IWebRequestTracer> sharedPointer = nullptr;
		std::shared_ptr<openkit::ILogger> logger = nullptr;
	} WebRequestTracerHandle;

	WebRequestTracerHandle* traceWebRequestOnSession(struct SessionHandle* sessionHandle, const char* url)
	{
		// Sanity
		if (sessionHandle == nullptr)
		{
			return nullptr;
		}

		WebRequestTracerHandle* handle = nullptr;
		TRY
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(sessionHandle->sharedPointer != nullptr);
			auto traceWebRequest = sessionHandle->sharedPointer->traceWebRequest(url);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new WebRequestTracerHandle();
			handle->sharedPointer = traceWebRequest;
			handle->logger = sessionHandle->logger;
		}
		CATCH_AND_LOG(sessionHandle)

		return handle;
	}

	WebRequestTracerHandle* traceWebRequestOnRootAction(RootActionHandle* rootActionHandle, const char* url)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return nullptr;
		}

		WebRequestTracerHandle* handle = nullptr;
		TRY
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(rootActionHandle->sharedPointer != nullptr);
			auto traceWebRequest = rootActionHandle->sharedPointer->traceWebRequest(url);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new WebRequestTracerHandle();
			handle->sharedPointer = traceWebRequest;
			handle->logger = rootActionHandle->logger;
		}
		CATCH_AND_LOG(rootActionHandle)

		return handle;
	}

	WebRequestTracerHandle* traceWebRequestOnAction(ActionHandle* actionHandle, const char* url)
	{
		// Sanity
		if (actionHandle == nullptr)
		{
			return nullptr;
		}

		WebRequestTracerHandle* handle = nullptr;
		TRY
		{
			// retrieve the Action instance from the handle and call the respective method
			assert(actionHandle->sharedPointer != nullptr);
			auto traceWebRequest = actionHandle->sharedPointer->traceWebRequest(url);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new WebRequestTracerHandle();
			handle->sharedPointer = traceWebRequest;
			handle->logger = actionHandle->logger;
		}
		CATCH_AND_LOG(actionHandle)

		return handle;
	}

	void startWebRequest(WebRequestTracerHandle* webRequestTracerHandle)
	{
		TRY
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->start();
			}
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

	///
	/// @deprecated use stopWebRequest(WebRequestTracerHandle*, int32_t) instead
	///
	OPENKIT_DEPRECATED
	void stopWebRequest(WebRequestTracerHandle* webRequestTracerHandle)
	{
		// Sanity
		if (webRequestTracerHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the WebRequestTracer instance from the handle and call the respective method
			assert(webRequestTracerHandle->sharedPointer != nullptr);
			webRequestTracerHandle->sharedPointer->stop();

			// release shared pointer
			webRequestTracerHandle->sharedPointer = nullptr;
			webRequestTracerHandle->logger = nullptr;
			delete webRequestTracerHandle;
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

	void stopWebRequestWithResponseCode(WebRequestTracerHandle* webRequestTracerHandle, int32_t responseCode)
	{
		// Sanity
		if(webRequestTracerHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the WebRequestTracer instance from the handle and call the respective method
			assert(webRequestTracerHandle->sharedPointer != nullptr);
			webRequestTracerHandle->sharedPointer->stop(responseCode);

			// release shared pointer
			webRequestTracerHandle->sharedPointer = nullptr;
			webRequestTracerHandle->logger = nullptr;
			delete webRequestTracerHandle;
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

	const char* getTag(WebRequestTracerHandle* webRequestTracerHandle)
	{
		TRY
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				return webRequestTracerHandle->sharedPointer->getTag();
			}
		}
		CATCH_AND_LOG(webRequestTracerHandle)

		return nullptr;
	}

	///
	/// @deprecated use stopWebRequest(WebRequestTracerHandle*, int32_t) instead
	///
	OPENKIT_DEPRECATED
	void setResponseCode(WebRequestTracerHandle* webRequestTracerHandle, int32_t responseCode)
	{
		TRY
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->setResponseCode(responseCode);
			}
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

	void setBytesSent(WebRequestTracerHandle* webRequestTracerHandle, int32_t bytesSent)
	{
		TRY
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->setBytesSent(bytesSent);
			}
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

	void setBytesReceived(WebRequestTracerHandle* webRequestTracerHandle, int32_t bytesReceived)
	{
		TRY
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->setBytesReceived(bytesReceived);
			}
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

} /* extern "C" */
