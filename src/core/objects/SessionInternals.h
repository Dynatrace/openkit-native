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

#ifndef _CORE_OBJECTS_SESSIONINTERNALS_H
#define _CORE_OBJECTS_SESSIONINTERNALS_H

#include "OpenKit/IRootAction.h"
#include "OpenKit/ISession.h"
#include "OpenKit/IWebRequestTracer.h"

#include "core/configuration/IBeaconConfiguration.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/OpenKitComposite.h"
#include "protocol/IBeacon.h"
#include "protocol/IStatusResponse.h"
#include "providers/IHTTPClientProvider.h"

#include <memory>

namespace core
{
	namespace objects
	{
		class SessionInternals
			: public core::objects::OpenKitComposite
			, public openkit::ISession
		{
		public:

			~SessionInternals() override = default;

			std::shared_ptr<openkit::IRootAction> enterAction(const char* actionName) override = 0;

			void identifyUser(const char* userTag) override = 0;

			void reportCrash(const char* errorName, const char* reason, const char* stacktrace) override = 0;

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url) override = 0;

			void end() override = 0;

			///
			/// Start a session
			///
			virtual void startSession() = 0;

			///
			/// Sends the current Beacon state
			/// @param[in] clientProvider the IHTTPClientProvider to use for sending
			/// @returns the status response returned for the Beacon data
			///
			virtual std::shared_ptr<protocol::IStatusResponse> sendBeacon(
				std::shared_ptr<providers::IHTTPClientProvider> clientProvider
			) = 0;

			///
			/// Test if this session is empty or not
			///
			/// A session is considered to be empty, if it does not contain any action or event data.
			/// @returns @c true if the session is empty, @c false otherwise
			///
			virtual bool isEmpty() const = 0;

			///
			/// Clears data that has been captured so far.
			///
			/// This is called, when capturing is turned off to avoid having too much data.
			///
			virtual void clearCapturedData() = 0;

			///
			/// Updates the this session with the given server configuration.
			///
			virtual void updateServerConfiguration(
				std::shared_ptr<core::configuration::IServerConfiguration> serverConfig
			) = 0;

			void onChildClosed(std::shared_ptr<IOpenKitObject> childObject) override = 0;

			void close() override = 0;

			///
			/// Indicates whether sending data for this session is allowed or not.
			///
			virtual bool isDataSendingAllowed() = 0;

			///
			/// Enables capturing for this session.
			///
			/// @par
			/// Will implicitly also set the state of this session to @ref isConfigured()
			///
			virtual void enableCapture() = 0;

			///
			/// Disables capturing for this session.
			///
			/// @par
			/// Will implicitly also set the state of this session to @ref isConfigured()
			///
			virtual void disableCapture() = 0;

			///
			/// Indicates whether new session requests can be sent or not.
			///
			/// @par
			/// This is directly related to @ref decreaseNumRemainingSessionRequests()
			///
			virtual bool canSendNewSessionRequest() const = 0;

			///
			/// Decreases the number of remaining new session requests
			///
			/// @par
			/// In case no more new session requests remain, @ref canSendNewSessionRequest() will return @c false
			///
			virtual void decreaseNumRemainingSessionRequests() = 0;

			///
			/// Returns the beacon associated with this session.
			///
			virtual std::shared_ptr<protocol::IBeacon> getBeacon() = 0;

			///
			/// Indicates whether this session is configured or not.
			///
			/// @par
			/// A session is considered as configured if it received configuration updates from the server.
			///
			virtual bool isConfigured() = 0;

			///
			/// Indicates if this session is finished and was configured.
			///
			virtual bool isConfiguredAndFinished() = 0;

			///
			/// Indicates if this session is configured and not yet finished.
			///
			virtual bool isConfiguredAndOpen() = 0;

			///
			/// Indicates if the session is finished.
			///
			/// @par
			/// A session is considered as finished, after @ref end() was called.
			///
			virtual bool isFinished() = 0;
		};
	}
}

#endif
