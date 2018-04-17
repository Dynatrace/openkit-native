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
		auto logger = std::shared_ptr<api::ILogger>(new apic::CustomLogger(levelEnabledFunc, logFunc));
		// storing the returned shared pointer in the handle prevents it from going out of scope
		LoggerHandle* handle = new LoggerHandle();
		handle->sharedPointer = logger;
		return handle;
	}

	void destroyLogger(LoggerHandle* loggerHandle)
	{
		// Sanity
		if (loggerHandle == nullptr)
		{
			return;
		}

		if (loggerHandle->sharedPointer)
		{
			// release shared pointer
			loggerHandle->sharedPointer = nullptr;
		}
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
		char test[] = "The answer to life";
		loggerHandle->sharedPointer->error("This is a test '%s' is %d", test, 42);
		api::DynatraceOpenKitBuilder builder(endpointURL, applicationID, deviceID);
		if (loggerHandle)
		{
			// Instantiate the CustomLogger mapping the log statements to the FunctionPointers
			builder.withLogger(loggerHandle->sharedPointer);
		}
		std::shared_ptr<api::IOpenKit> openKit = builder.build();
		
		// storing the returned shared pointer in the handle prevents it from going out of scope
		OpenKitHandle* handle = new OpenKitHandle();
		handle->sharedPointer = openKit;
		return handle;
	}

	OpenKitHandle* createAppMonOpenKit(const char* endpointURL, const char* applicationID, int64_t deviceID, LoggerHandle* loggerHandle)
	{
		api::AppMonOpenKitBuilder builder(endpointURL, applicationID, deviceID);
		if (loggerHandle)
		{
			// Instantiate the CustomLogger mapping the log statements to the FunctionPointers
			builder.withLogger(loggerHandle->sharedPointer);
		}
		std::shared_ptr<api::IOpenKit> openKit = builder.build();

		// storing the returned shared pointer in the handle prevents it from going out of scope
		OpenKitHandle* handle = new OpenKitHandle();
		handle->sharedPointer = openKit;
		return handle;
	}

	void shutdownOpenKit(OpenKitHandle* openKitHandle)
	{
		// Sanity
		if (openKitHandle == nullptr)
		{
			return;
		}

		if (openKitHandle->sharedPointer)
		{
			std::shared_ptr<api::IOpenKit> openKit = openKitHandle->sharedPointer;
			openKit->shutdown();

			// release shared pointer
			openKitHandle->sharedPointer = nullptr;
		}
		delete openKitHandle;
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

		// retrieve the OpenKit instance from the handle and call the respective method
		std::shared_ptr<api::ISession> session = openKitHandle->sharedPointer->createSession(clientIPAddress);

		// storing the returned shared pointer in the handle prevents it from going out of scope
		SessionHandle* handle = new SessionHandle();
		handle->sharedPointer = session;
		return handle;
	}

	void endSession(SessionHandle* sessionHandle)
	{
		// Sanity
		if (sessionHandle == nullptr)
		{
			return;
		}

		if (sessionHandle->sharedPointer)
		{
			sessionHandle->sharedPointer->end();

			// release shared pointer
			sessionHandle->sharedPointer = nullptr;
		}
		delete sessionHandle;
	}

	void identifyUser(SessionHandle* sessionHandle, const char* userTag)
	{
		if (sessionHandle && sessionHandle->sharedPointer)
		{
			sessionHandle->sharedPointer->identifyUser(userTag);
		}
	}

	void reportCrash(SessionHandle* sessionHandle, const char* errorName, const char* reason, const char* stacktrace)
	{
		if (sessionHandle && sessionHandle->sharedPointer)
		{
			sessionHandle->sharedPointer->reportCrash(errorName, reason, stacktrace);
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

		// retrieve the OpenKit instance from the handle and call the respective method
		std::shared_ptr<api::IRootAction> rootAction = sessionHandle->sharedPointer->enterAction(rootActionName);

		// storing the returned shared pointer in the handle prevents it from going out of scope
		RootActionHandle* handle = new RootActionHandle();
		handle->sharedPointer = rootAction;
		return handle;
	}

	void leaveRootAction(RootActionHandle* rootActionHandle)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return;
		}

		if (rootActionHandle->sharedPointer)
		{
			rootActionHandle->sharedPointer->leaveAction();

			// release shared pointer
			rootActionHandle->sharedPointer = nullptr;
		}
		delete rootActionHandle;
	}

	void reportEventOnRootAction(RootActionHandle* rootActionHandle, const char* eventName)
	{
		if (rootActionHandle && rootActionHandle->sharedPointer)
		{
			rootActionHandle->sharedPointer->reportEvent(eventName);
		}
	}

	void reportIntValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, int32_t value)
	{
		if (rootActionHandle && rootActionHandle->sharedPointer)
		{
			rootActionHandle->sharedPointer->reportValue(valueName, value);
		}
	}

	void reportDoubleValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, double value)
	{
		if (rootActionHandle && rootActionHandle->sharedPointer)
		{
			rootActionHandle->sharedPointer->reportValue(valueName, value);
		}
	}

	void reportStringValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, const char* value)
	{
		if (rootActionHandle && rootActionHandle->sharedPointer)
		{
			rootActionHandle->sharedPointer->reportValue(valueName, value);
		}
	}

	void reportErrorOnRootAction(RootActionHandle* rootActionHandle, const char* errorName, int32_t errorCode, const char* reason)
	{
		if (rootActionHandle && rootActionHandle->sharedPointer)
		{
			rootActionHandle->sharedPointer->reportError(errorName, errorCode, reason);
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

		// retrieve the OpenKit instance from the handle and call the respective method
		std::shared_ptr<api::IAction> action = rootActionHandle->sharedPointer->enterAction(actionName);
		
		// storing the returned shared pointer in the handle prevents it from going out of scope
		ActionHandle* handle = new ActionHandle();
		handle->sharedPointer = action;
		return handle;
	}

	void leaveAction(ActionHandle* actionHandle)
	{
		// Sanity
		if (actionHandle == nullptr)
		{
			return;
		}

		if (actionHandle->sharedPointer)
		{
			actionHandle->sharedPointer->leaveAction();

			// release shared pointer
			actionHandle->sharedPointer = nullptr;
		}
		delete actionHandle;
	}

	void reportEventOnAction(ActionHandle* actionHandle, const char* eventName)
	{
		if (actionHandle && actionHandle->sharedPointer)
		{
			actionHandle->sharedPointer->reportEvent(eventName);
		}
	}

	void reportIntValueOnAction(ActionHandle* actionHandle, const char* valueName, int32_t value)
	{
		if (actionHandle && actionHandle->sharedPointer)
		{
			actionHandle->sharedPointer->reportValue(valueName, value);
		}
	}

	void reportDoubleValueOnAction(ActionHandle* actionHandle, const char* valueName, double value)
	{
		if (actionHandle && actionHandle->sharedPointer)
		{
			actionHandle->sharedPointer->reportValue(valueName, value);
		}
	}
	
	void reportStringValueOnAction(ActionHandle* actionHandle, const char* valueName, const char* value)
	{
		if (actionHandle && actionHandle->sharedPointer)
		{
			actionHandle->sharedPointer->reportValue(valueName, value);
		}
	}

	void reportErrorOnAction(ActionHandle* actionHandle, const char* errorName, int32_t errorCode, const char* reason)
	{
		if (actionHandle && actionHandle->sharedPointer)
		{
			actionHandle->sharedPointer->reportError(errorName, errorCode, reason);
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
		if (rootActionHandle && rootActionHandle->sharedPointer)
		{
			auto traceWebRequest = rootActionHandle->sharedPointer->traceWebRequest(url);
			
			// storing the returned shared pointer in the handle prevents it from going out of scope
			WebRequestTracerHandle* handle = new WebRequestTracerHandle();
			handle->sharedPointer = traceWebRequest;
			return handle;
		}

		return nullptr;
	}

	WebRequestTracerHandle* traceWebRequestOnAction(ActionHandle* actionHandle, const char* url)
	{
		if (actionHandle && actionHandle->sharedPointer)
		{
			auto traceWebRequest = actionHandle->sharedPointer->traceWebRequest(url);
			
			// storing the returned shared pointer in the handle prevents it from going out of scope
			WebRequestTracerHandle* handle = new WebRequestTracerHandle();
			handle->sharedPointer = traceWebRequest;
			return handle;
		}

		return nullptr;
	}

	void stop(WebRequestTracerHandle* webRequestTracerHandle)
	{
		// Sanity
		if (webRequestTracerHandle == nullptr)
		{
			return;
		}

		if (webRequestTracerHandle->sharedPointer)
		{
			webRequestTracerHandle->sharedPointer->stop();

			// release shared pointer
			webRequestTracerHandle->sharedPointer = nullptr;
		}
		delete webRequestTracerHandle;
	}

	const char* getTag(WebRequestTracerHandle* webRequestTracerHandle)
	{
		if (webRequestTracerHandle && webRequestTracerHandle->sharedPointer)
		{
			return webRequestTracerHandle->sharedPointer->getTag();
		}

		return nullptr;
	}

	void setResponseCode(WebRequestTracerHandle* webRequestTracerHandle, int32_t responseCode)
	{
		if (webRequestTracerHandle && webRequestTracerHandle->sharedPointer)
		{
			webRequestTracerHandle->sharedPointer->setResponseCode(responseCode);
		}
	}

	void setBytesSent(WebRequestTracerHandle* webRequestTracerHandle, int32_t bytesSent)
	{
		if (webRequestTracerHandle && webRequestTracerHandle->sharedPointer)
		{
			webRequestTracerHandle->sharedPointer->setBytesSent(bytesSent);
		}
	}

	void setBytesReceived(WebRequestTracerHandle* webRequestTracerHandle, int32_t bytesReceived)
	{
		if (webRequestTracerHandle && webRequestTracerHandle->sharedPointer)
		{
			webRequestTracerHandle->sharedPointer->setBytesReceived(bytesReceived);
		}
	}

} /* extern "C" */
