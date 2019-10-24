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
			/// Returns the end time of the session
			/// @returns the end time of the session
			///
			virtual int64_t getEndTime() const = 0;

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
			/// Return a flag if this session was ended already
			/// @returns @c true if session was already ended, @c false if session is still open
			///
			virtual bool isSessionEnded() const = 0;

			///
			/// Sets the beacon configuration
			/// @param[in] beaconConfiguration the beacon configuration to apply to the Beacon
			///
			virtual void setBeaconConfiguration(
				std::shared_ptr<configuration::IBeaconConfiguration> beaconConfiguration
			) = 0;

			///
			/// Returns the beacon configuration
			/// @returns the beacon configuration
			///
			virtual std::shared_ptr<configuration::IBeaconConfiguration> getBeaconConfiguration() const = 0;

			void onChildClosed(std::shared_ptr<IOpenKitObject> childObject) override = 0;

			void close() override = 0;
		};
	}
}

#endif
