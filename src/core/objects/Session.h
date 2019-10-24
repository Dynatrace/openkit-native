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

#ifndef _CORE_OBJECTS_SESSION_H
#define _CORE_OBJECTS_SESSION_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/ISession.h"
#include "OpenKit/IRootAction.h"
#include "OpenKit/ILogger.h"
#include "NullRootAction.h"

#include "core/UTF8String.h"
#include "core/IBeaconSender.h"
#include "core/configuration/IBeaconConfiguration.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/SessionInternals.h"
#include "core/util/SynchronizedQueue.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/IHTTPClientProvider.h"

#include <memory>
#include <atomic>

namespace protocol
{
	class IBeacon;
}

namespace core {
	class BeaconSender;
}

namespace core
{
	namespace objects
	{
		///
		///  Actual implementation of the SessionInternals "interface".
		///
		class Session
			: public SessionInternals
			, public std::enable_shared_from_this<Session>
		{
		public:

			///
			/// Constructor
			/// @param[in] logger to write traces to
			/// @param[in] beaconSender beacon sender
			/// @param[in] beacon beacon used for serialization
			///
			Session(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<IBeaconSender> beaconSender,
				std::shared_ptr<protocol::IBeacon> beacon
			);

			///
			/// Destructor
			///
			~Session() override = default;

			std::shared_ptr<openkit::IRootAction> enterAction(const char* actionName) override;

			void identifyUser(const char* userTag) override;

			void reportCrash(const char* errorName, const char* reason, const char* stacktrace) override;

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url) override;

			void end() override;

			///
			/// Returns the end time of the session
			/// @returns the end time of the session
			///
			int64_t getEndTime() const override;

			///
			/// Start a session
			///
			void startSession() override;

			///
			/// Sends the current Beacon state
			/// @param[in] clientProvider the IHTTPClientProvider to use for sending
			/// @returns the status response returned for the Beacon data
			///
			std::shared_ptr<protocol::IStatusResponse> sendBeacon(
				std::shared_ptr<providers::IHTTPClientProvider> clientProvider
			) override;

			///
			/// Test if this session is empty or not
			///
			/// A session is considered to be empty, if it does not contain any action or event data.
			/// @returns @c true if the session is empty, @c false otherwise
			///
			bool isEmpty() const override;


			///
			/// Clears data that has been captured so far.
			///
			/// This is called, when capturing is turned off to avoid having too much data.
			///
			void clearCapturedData() override;

			///
			/// Return a flag if this session was ended already
			/// @returns @c true if session was already ended, @c false if session is still open
			///
			bool isSessionEnded() const override;

			///
			/// Sets the beacon configuration
			/// @param[in] beaconConfiguration the beacon configuration to apply to the Beacon
			///
			void setBeaconConfiguration(
				std::shared_ptr<configuration::IBeaconConfiguration> beaconConfiguration
			) override;

			///
			/// Returns the beacon configuration
			/// @returns the beacon configuration
			///
			std::shared_ptr<configuration::IBeaconConfiguration> getBeaconConfiguration() const override;

			void onChildClosed(std::shared_ptr<IOpenKitObject> childObject) override;

			void close() override;

		private:
			///
			/// Returns a string describing the object, based on some important fields.
			/// This function is indended for debug printouts.
			/// @return a string describing the object
			///
			const std::string toString() const;

			/// Logger to write traces to
			std::shared_ptr<openkit::ILogger> mLogger;

			/// beacon sender
			std::shared_ptr<IBeaconSender> mBeaconSender;

			/// beacon used for serialization
			std::shared_ptr<protocol::IBeacon> mBeacon;

			/// end time
			std::atomic<int64_t> mEndTime;
		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
