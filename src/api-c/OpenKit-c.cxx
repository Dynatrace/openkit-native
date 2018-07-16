/**
* Copyright 2018 Dynatrace LLC
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


#include "OpenKit.h"

#include "api-c/OpenKit-c.h"
#include "api-c/CustomLogger.h"
#include "api-c/CustomTrustManager.h"

#include "core/util/DefaultLogger.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "protocol/ssl/SSLBlindTrustManager.h"

#include <list>
#include <assert.h> 

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
		catch (...)
		{
			/* Ignore exception, as we don't have a logger yet */
		}

		return handle;
	}

	TrustManagerHandle* createStrictTrustManager()
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
		catch (...)
		{
			/* Ignore exception, as we don't have a logger yet */
		}

		return handle;
	}

	TrustManagerHandle* createBlindTrustManager()
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
		catch (...)
		{
			/* Ignore exception, as we don't have a logger yet */
		}

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
			auto logger = std::shared_ptr<openkit::ILogger>(new apic::CustomLogger(levelEnabledFunc, logFunc));
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new LoggerHandle();
			handle->logger = logger;
		}
		catch (...)
		{
			/* Ignore exception, as we don't have a logger yet */
		}

		return handle;
	}

	LoggerHandle* createDefaultLogger()
	{
		LoggerHandle* handle = nullptr;
		try
		{
			auto logger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(true));
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new LoggerHandle();
			handle->logger = logger;
		}
		catch (...)
		{
			/* Ignore exception, as we don't have a logger yet */
		}

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

	//-----------------------
	//  OpenKit configuration
	//-----------------------

	typedef struct OpenKitConfigurationHandle
	{
		const char* endpointURL;
		const char* applicationID;
		int64_t deviceID;
		const char* applicationName;
		LoggerHandle* loggerHandle;
		bool ownsLoggerHandle;
		const char* applicationVersion;
		TRUST_MODE trustMode;
		TrustManagerHandle* trustManagerHandle;
		bool ownsTrustManagerHandle;
		const char* operatingSystem;
		const char* manufacturer;
		const char* modelID;
		int64_t beaconCacheMaxRecordAge;
		int64_t beaconCacheLowerMemoryBoundary;
		int64_t beaconCacheUpperMemoryBoundary;
	} OpenKitConfigurationHandle ;

	struct OpenKitConfigurationHandle* createOpenKitConfiguration(const char* endpointURL, const char* applicationID, int64_t deviceID)
	{
		OpenKitConfigurationHandle* handle = nullptr;
		try
		{
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new OpenKitConfigurationHandle();
			handle->endpointURL = endpointURL;
			handle->applicationID = applicationID;
			handle->applicationName = nullptr;
			handle->deviceID = deviceID;
			handle->loggerHandle = nullptr;
			handle->ownsLoggerHandle = false;
			handle->applicationVersion = nullptr;
			handle->trustMode = STRICT_TRUST;
			handle->trustManagerHandle = nullptr;
			handle->ownsTrustManagerHandle = false;
			handle->operatingSystem = nullptr;
			handle->manufacturer = nullptr;
			handle->modelID = nullptr;
			handle->beaconCacheMaxRecordAge = -1;
			handle->beaconCacheLowerMemoryBoundary = -1;
			handle->beaconCacheUpperMemoryBoundary = -1;
		}
		catch (...)
		{
			/* Ignore exception, as we don't have a logger yet */
		}

		return handle;
	}

	void destroyOpenKitConfiguration(struct OpenKitConfigurationHandle* configurationHandle)
	{
		// Sanity
		if (configurationHandle == nullptr)
		{
			return;
		}

		// release shared pointer
		configurationHandle = nullptr;
		delete configurationHandle;
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
			configurationHandle->applicationVersion = applicationVersion;
		}
	}

	void useApplicationNameForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* applicationName)
	{
		//sanity
		if (configurationHandle != nullptr && applicationName != nullptr)
		{
			configurationHandle->applicationName = applicationName;
		}
	}

	void useTrustModeForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, TRUST_MODE trustMode, struct TrustManagerHandle* trustManagerHandle)
	{
		//sanity
		if (configurationHandle != nullptr && trustManagerHandle != nullptr)
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
			configurationHandle->operatingSystem = operatingSystem;
		}
	}

	void useManufacturerForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* manufacturer)
	{
		//sanity
		if (configurationHandle != nullptr && manufacturer != nullptr)
		{
			configurationHandle->manufacturer = manufacturer;
		}
	}

	void useModelIDForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, const char* modelID)
	{
		//sanity
		if (configurationHandle != nullptr && modelID != nullptr)
		{
			configurationHandle->modelID = modelID;
		}
	}

	void useBeaconCacheBehaviorForConfiguration(struct OpenKitConfigurationHandle* configurationHandle, int64_t beaconCacheMaxRecordAge, int64_t beaconCacheLowerMemoryBoundary, int64_t beaconCacheUpperMemoryBoundary)
	{
		//sanity
		if (configurationHandle != nullptr && beaconCacheMaxRecordAge != -1)
		{
			configurationHandle->beaconCacheMaxRecordAge = beaconCacheMaxRecordAge;
		}
		if (configurationHandle != nullptr && beaconCacheLowerMemoryBoundary)
		{
			configurationHandle->beaconCacheLowerMemoryBoundary = beaconCacheLowerMemoryBoundary;
		}
		if (configurationHandle != nullptr && beaconCacheLowerMemoryBoundary)
		{
			configurationHandle->beaconCacheUpperMemoryBoundary = beaconCacheUpperMemoryBoundary;
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
			struct LoggerHandle* defaultLogger = createDefaultLogger();

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
	}

	static OpenKitHandle* createOpenKitHandle(struct OpenKitConfigurationHandle* configurationHandle, std::shared_ptr<openkit::IOpenKit> openKit)
	{
		// storing the returned shared pointer in the handle prevents it from going out of scope
		OpenKitHandle* handle = new OpenKitHandle();
		handle->sharedPointer = openKit;
		handle->logger = configurationHandle->loggerHandle->logger;
		handle->loggerHandle = configurationHandle->loggerHandle;
		handle->ownsLoggerHandle = configurationHandle->ownsLoggerHandle;
		handle->trustManagerHandle = configurationHandle->trustManagerHandle;
		handle->ownsTrustManagerHandle = configurationHandle->ownsTrustManagerHandle;

		return handle;
	}

	OpenKitHandle* createDynatraceOpenKit(struct OpenKitConfigurationHandle* configurationHandle)
	{
		OpenKitHandle* handle = nullptr;
		TRY
		{
			openkit::DynatraceOpenKitBuilder builder(configurationHandle->endpointURL, configurationHandle->applicationID, configurationHandle->deviceID);
			if (configurationHandle->applicationName != nullptr)
			{
				builder.withApplicationName(configurationHandle->applicationName);
			}

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
			openkit::AppMonOpenKitBuilder builder(configurationHandle->endpointURL, configurationHandle->applicationName, configurationHandle->deviceID);

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

			// release shared pointer
			rootActionHandle->sharedPointer = nullptr;
			rootActionHandle->logger = nullptr;
			delete rootActionHandle;
		}
		CATCH_AND_LOG(rootActionHandle)
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

	void reportErrorOnRootAction(RootActionHandle* rootActionHandle, const char* errorName, int32_t errorCode, const char* reason)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportError(errorName, errorCode, reason);
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

			// release shared pointer
			actionHandle->sharedPointer = nullptr;
			actionHandle->logger = nullptr;
			delete actionHandle;
		}
		CATCH_AND_LOG(actionHandle)
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

	void reportErrorOnAction(ActionHandle* actionHandle, const char* errorName, int32_t errorCode, const char* reason)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportError(errorName, errorCode, reason);
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
