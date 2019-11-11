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
#include "core/objects/IOpenKitComposite.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/SessionInternals.h"
#include "core/util/SynchronizedQueue.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/IHTTPClientProvider.h"

#include <memory>
#include <atomic>
#include <mutex>

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
			/// @param[in] parent the parent composite of this session
			/// @param[in] beacon beacon used for serialization
			///
			Session(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<core::objects::IOpenKitComposite> parent,
				std::shared_ptr<protocol::IBeacon> beacon
			);

			///
			/// Destructor
			///
			~Session() override = default;

			///
			/// The maximum number of "new session requests" to send per session.
			///
			static constexpr int32_t MAX_NEW_SESSION_REQUESTS = 4;

			std::shared_ptr<openkit::IRootAction> enterAction(const char* actionName) override;

			void identifyUser(const char* userTag) override;

			void reportCrash(const char* errorName, const char* reason, const char* stacktrace) override;

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url) override;

			void end() override;

			void startSession() override;

			std::shared_ptr<protocol::IStatusResponse> sendBeacon(
				std::shared_ptr<providers::IHTTPClientProvider> clientProvider
			) override;

			bool isEmpty() const override;



			void clearCapturedData() override;

			void updateServerConfiguration(
				std::shared_ptr<core::configuration::IServerConfiguration> serverConfig
			) override;

			///
			/// Returns a string describing the object, based on some important fields.
			/// This function is intended for debug printouts.
			/// @return a string describing the object
			///
			const std::string toString() const;

			void onChildClosed(std::shared_ptr<IOpenKitObject> childObject) override;

			void close() override;

			bool isDataSendingAllowed() override;

			void enableCapture() override;

			void disableCapture() override;

			bool canSendNewSessionRequest() const override;

			void decreaseNumRemainingSessionRequests() override;

			bool isConfigured() override;

			bool isConfiguredAndFinished() override;

			bool isConfiguredAndOpen() override;

			bool isFinished() override;

		private:

			///
			/// Marks that the session is about to be finished.
			///
			/// @return @c true if the session was marked for finishing, @c false if the session was already previously
			///  finished or marked for finishing
			bool markAsIsFinishing();

			///
			/// Marks the session as finished.
			///
			void markAsFinished();

			///
			/// Indicates if this session is configured.
			///
			bool hasServerConfigurationSet();

			///
			/// Indicates if the session is finished or currently finishing.
			///
			bool isFinishingOrFinished();

		private:
			/// Logger to write traces to
			const std::shared_ptr<openkit::ILogger> mLogger;

			// parent object of this session
			const std::shared_ptr<core::objects::IOpenKitComposite> mParent;

			/// beacon used for serialization
			const std::shared_ptr<protocol::IBeacon> mBeacon;

			/// the number of tries for new session requests.
			int32_t mNumRemainingNewSessionRequests;

			/// indicator that the session is currently finishing/finished.
			std::atomic<bool> mIsSessionFinishing;

			/// indicator if the session was ended
			std::atomic<bool> mIsSessionFinished;

			// mutex used for synchronization
			std::mutex mMutex;
		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
