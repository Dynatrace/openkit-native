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


#include "api/IOpenKit.h"
#include "api/ILogger.h"
#include "api/ISession.h"
#include "api/IRootAction.h"
#include "api/IAction.h"
#include "api/AppMonOpenKitBuilder.h"
#include "api/DynatraceOpenKitBuilder.h"

#include "api-c/OpenKit-c.h"
#include "api-c/CustomLogger.h"

#include <list>
#include <assert.h> 

extern "C" {

	//--------------
	//  Logger
	//--------------

	typedef struct LoggerHandle
	{
		std::shared_ptr<api::ILogger> sharedPointer = nullptr;
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
			auto logger = std::shared_ptr<api::ILogger>(new apic::CustomLogger(levelEnabledFunc, logFunc));
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new LoggerHandle();
			handle->sharedPointer = logger;
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
		loggerHandle->sharedPointer = nullptr;
		delete loggerHandle;
	}

	//--------------
	//  OpenKit
	//--------------

	typedef struct OpenKitHandle
	{
		std::shared_ptr<api::IOpenKit> sharedPointer = nullptr;
	} OpenKitHandle;

	OpenKitHandle* createDynatraceOpenKit(const char* endpointURL, const char* applicationID, int64_t deviceID, LoggerHandle* loggerHandle)
	{
		OpenKitHandle* handle = nullptr;
		try
		{
			api::DynatraceOpenKitBuilder builder(endpointURL, applicationID, deviceID);
			if (loggerHandle)
			{
				// Instantiate the CustomLogger mapping the log statements to the FunctionPointers
				builder.withLogger(loggerHandle->sharedPointer);
			}
			std::shared_ptr<api::IOpenKit> openKit = builder.build();
		
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new OpenKitHandle();
			handle->sharedPointer = openKit;
		}
		catch (const std::exception& ex)
		{
			if (loggerHandle)
			{
				assert(loggerHandle->sharedPointer != nullptr);
				loggerHandle->sharedPointer->error("Exception occurred in %s #%d: %s", __FILE__, __LINE__, ex.what());
			}
		}
		catch (...)
		{ 
			if (loggerHandle)
			{
				assert(loggerHandle->sharedPointer != nullptr);
				loggerHandle->sharedPointer->error("Unknown exception occurred in %s #%d", __FILE__, __LINE__);
			}
		}

		return handle;
	}

	OpenKitHandle* createAppMonOpenKit(const char* endpointURL, const char* applicationID, int64_t deviceID, LoggerHandle* loggerHandle)
	{
		OpenKitHandle* handle = nullptr;
		try
		{
			api::AppMonOpenKitBuilder builder(endpointURL, applicationID, deviceID);
			if (loggerHandle)
			{
				// Instantiate the CustomLogger mapping the log statements to the FunctionPointers
				builder.withLogger(loggerHandle->sharedPointer);
			}
			std::shared_ptr<api::IOpenKit> openKit = builder.build();

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new OpenKitHandle();
			handle->sharedPointer = openKit;
		}
		catch (const std::exception& ex)
		{
			if (loggerHandle)
			{
				assert(loggerHandle->sharedPointer != nullptr);
				loggerHandle->sharedPointer->error("Exception occurred in %s #%d: %s", __FILE__, __LINE__, ex.what());
			}
		}
		catch (...)
		{
			if (loggerHandle)
			{
				assert(loggerHandle->sharedPointer != nullptr);
				loggerHandle->sharedPointer->error("Unknown exception occurred in %s #%d", __FILE__, __LINE__);
			}
		}
		return handle;
	}

	void shutdownOpenKit(OpenKitHandle* openKitHandle)
	{
		// Sanity
		if (openKitHandle == nullptr)
		{
			return;
		}

		try
		{
			// retrieve the OpenKit instance from the handle and call the respective method
			assert(openKitHandle->sharedPointer != nullptr);
			openKitHandle->sharedPointer->shutdown();

			// release shared pointer
			openKitHandle->sharedPointer = nullptr;
			delete openKitHandle;
		}
		catch (...)
		{
		}
	}


	//--------------
	//  Session
	//--------------

	typedef struct SessionHandle
	{
		std::shared_ptr<api::ISession> sharedPointer = nullptr;
	} SessionHandle;

	SessionHandle* createSession(OpenKitHandle* openKitHandle, const char* clientIPAddress)
	{
		// Sanity
		if (openKitHandle == nullptr)
		{
			return nullptr;
		}

		SessionHandle* handle = nullptr;
		try
		{
			// retrieve the OpenKit instance from the handle and call the respective method
			assert(openKitHandle->sharedPointer != nullptr);
			std::shared_ptr<api::ISession> session = openKitHandle->sharedPointer->createSession(clientIPAddress);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new SessionHandle();
			handle->sharedPointer = session;
		}
		catch (...)
		{
		}
		return handle;
	}

	void endSession(SessionHandle* sessionHandle)
	{
		// Sanity
		if (sessionHandle == nullptr)
		{
			return;
		}

		try
		{
			// retrieve the Session instance from the handle and call the respective method
			assert(sessionHandle->sharedPointer != nullptr);
			sessionHandle->sharedPointer->end();

			// release shared pointer
			sessionHandle->sharedPointer = nullptr;
			delete sessionHandle;
		}
		catch (...)
		{
		}
	}

	void identifyUser(SessionHandle* sessionHandle, const char* userTag)
	{
		try
		{
			if (sessionHandle)
			{
				// retrieve the Session instance from the handle and call the respective method
				assert(sessionHandle->sharedPointer != nullptr);
				sessionHandle->sharedPointer->identifyUser(userTag);
			}
		}
		catch (...)
		{
		}
	}

	void reportCrash(SessionHandle* sessionHandle, const char* errorName, const char* reason, const char* stacktrace)
	{
		try
		{
			if (sessionHandle)
			{
				// retrieve the Session instance from the handle and call the respective method
				assert(sessionHandle->sharedPointer != nullptr);
				sessionHandle->sharedPointer->reportCrash(errorName, reason, stacktrace);
			}
		}
		catch (...)
		{
		}
	}

	//--------------
	//  Root Action
	//--------------

	typedef struct RootActionHandle
	{
		std::shared_ptr<api::IRootAction> sharedPointer = nullptr;
	} RootActionHandle;

	RootActionHandle* enterRootAction(SessionHandle* sessionHandle, const char* rootActionName)
	{
		// Sanity
		if (sessionHandle == nullptr)
		{
			return nullptr;
		}

		RootActionHandle* handle = nullptr;
		try
		{
			// retrieve the Session instance from the handle and call the respective method
			assert(sessionHandle->sharedPointer != nullptr);
			std::shared_ptr<api::IRootAction> rootAction = sessionHandle->sharedPointer->enterAction(rootActionName);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new RootActionHandle();
			handle->sharedPointer = rootAction;
		}
		catch (...)
		{
		}
		return handle;
	}

	void leaveRootAction(RootActionHandle* rootActionHandle)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return;
		}

		try
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(rootActionHandle->sharedPointer != nullptr);
			rootActionHandle->sharedPointer->leaveAction();

			// release shared pointer
			rootActionHandle->sharedPointer = nullptr;
			delete rootActionHandle;
		}
		catch (...)
		{
		}
	}

	void reportEventOnRootAction(RootActionHandle* rootActionHandle, const char* eventName)
	{
		try
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportEvent(eventName);
			}
		}
		catch (...)
		{
		}
	}

	void reportIntValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, int32_t value)
	{
		try
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		catch (...)
		{
		}
	}

	void reportDoubleValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, double value)
	{
		try
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		catch (...)
		{
		}
	}

	void reportStringValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, const char* value)
	{
		try
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		catch (...)
		{
		}
	}

	void reportErrorOnRootAction(RootActionHandle* rootActionHandle, const char* errorName, int32_t errorCode, const char* reason)
	{
		try
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportError(errorName, errorCode, reason);
			}
		}
		catch (...)
		{
		}
	}


	//--------------
	//  Action
	//--------------

	typedef struct ActionHandle
	{
		std::shared_ptr<api::IAction> sharedPointer = nullptr;
	} ActionHandle;

	ActionHandle* enterAction(RootActionHandle* rootActionHandle, const char* actionName)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return nullptr;
		}

		ActionHandle* handle = nullptr;
		try
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(rootActionHandle->sharedPointer != nullptr);
			std::shared_ptr<api::IAction> action = rootActionHandle->sharedPointer->enterAction(actionName);
		
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new ActionHandle();
			handle->sharedPointer = action;
		}
		catch (...)
		{
		}
		return handle;
	}

	void leaveAction(ActionHandle* actionHandle)
	{
		// Sanity
		if (actionHandle == nullptr)
		{
			return;
		}

		try
		{
			// retrieve the Action instance from the handle and call the respective method
			assert(actionHandle->sharedPointer != nullptr);
			actionHandle->sharedPointer->leaveAction();

			// release shared pointer
			actionHandle->sharedPointer = nullptr;
			delete actionHandle;
		}
		catch (...)
		{
		}
	}

	void reportEventOnAction(ActionHandle* actionHandle, const char* eventName)
	{
		try
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportEvent(eventName);
			}
		}
		catch (...)
		{
		}
	}

	void reportIntValueOnAction(ActionHandle* actionHandle, const char* valueName, int32_t value)
	{
		try
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		catch (...)
		{
		}
	}

	void reportDoubleValueOnAction(ActionHandle* actionHandle, const char* valueName, double value)
	{
		try
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		catch (...)
		{
		}
	}
	
	void reportStringValueOnAction(ActionHandle* actionHandle, const char* valueName, const char* value)
	{
		try
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		catch (...)
		{
		}
	}

	void reportErrorOnAction(ActionHandle* actionHandle, const char* errorName, int32_t errorCode, const char* reason)
	{
		try
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportError(errorName, errorCode, reason);
			}
		}
		catch (...)
		{
		}
	}


	//--------------------
	//  Webrequest Tracer
	//--------------------

	typedef struct WebRequestTracerHandle
	{
		std::shared_ptr<api::IWebRequestTracer> sharedPointer = nullptr;
	} WebRequestTracerHandle;

	WebRequestTracerHandle* traceWebRequestOnRootAction(RootActionHandle* rootActionHandle, const char* url)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return nullptr;
		}

		WebRequestTracerHandle* handle = nullptr;
		try
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(rootActionHandle->sharedPointer != nullptr);
			auto traceWebRequest = rootActionHandle->sharedPointer->traceWebRequest(url);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new WebRequestTracerHandle();
			handle->sharedPointer = traceWebRequest;
		}
		catch (...)
		{
		}
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
		try
		{
			// retrieve the Action instance from the handle and call the respective method
			assert(actionHandle->sharedPointer != nullptr);
			auto traceWebRequest = actionHandle->sharedPointer->traceWebRequest(url);
			
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new WebRequestTracerHandle();
			handle->sharedPointer = traceWebRequest;
		}
		catch (...)
		{
		}
		return handle;
	}

	void stop(WebRequestTracerHandle* webRequestTracerHandle)
	{
		
		// Sanity
		if (webRequestTracerHandle == nullptr)
		{
			return;
		}

		try
		{
			// retrieve the WebRequestTracer instance from the handle and call the respective method
			assert(webRequestTracerHandle->sharedPointer != nullptr);
			webRequestTracerHandle->sharedPointer->stop();

			// release shared pointer
			webRequestTracerHandle->sharedPointer = nullptr;
			delete webRequestTracerHandle;
		}
		catch (...)
		{
		}
	}

	const char* getTag(WebRequestTracerHandle* webRequestTracerHandle)
	{
		try
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				return webRequestTracerHandle->sharedPointer->getTag();
			}
		}
		catch (...)
		{
		}

		return nullptr;
	}

	void setResponseCode(WebRequestTracerHandle* webRequestTracerHandle, int32_t responseCode)
	{
		try
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->setResponseCode(responseCode);
			}
		}
		catch (...)
		{
		}
	}

	void setBytesSent(WebRequestTracerHandle* webRequestTracerHandle, int32_t bytesSent)
	{
		try
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->setBytesSent(bytesSent);
			}
		}
		catch (...)
		{
		}
	}

	void setBytesReceived(WebRequestTracerHandle* webRequestTracerHandle, int32_t bytesReceived)
	{
		try
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->setBytesReceived(bytesReceived);
			}
		}
		catch (...)
		{
		}
	}

} /* extern "C" */
