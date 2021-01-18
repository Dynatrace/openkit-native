/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _CORE_OBJECTS_SESSIONPROXY_H
#define _CORE_OBJECTS_SESSIONPROXY_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/ILogger.h"
#include "OpenKit/IRootAction.h"
#include "OpenKit/IWebRequestTracer.h"

#include "core/IBeaconSender.h"
#include "core/ISessionWatchdog.h"
#include "core/objects/ISessionProxy.h"
#include "core/objects/IOpenKitComposite.h"
#include "core/objects/ISessionCreator.h"
#include "core/objects/SessionInternals.h"
#include "core/objects/OpenKitComposite.h"
#include "core/configuration/IServerConfiguration.h"
#include "core/UTF8String.h"
#include "providers/ITimingProvider.h"

#include <memory>
#include <mutex>
#include <string>
#include <cstdint>

namespace core
{
	namespace objects
	{
		class SessionProxy
			: public core::objects::ISessionProxy
			, public OpenKitComposite
			, public std::enable_shared_from_this<SessionProxy>
		{
		public:

			///
			/// Creates a new SessionProxy instance.
			/// 
			/// @remarks
			/// Since the SessionProxy needs to create an initial Session immediately and
			/// @c shared_from_this is not possible in ctor, this helper method is introduced.
			/// It creates the new instance, as well as an initial Session.
			///
			static std::shared_ptr<SessionProxy> createSessionProxy(std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<IOpenKitComposite> parent,
				std::shared_ptr<ISessionCreator> sessionCreator,
				std::shared_ptr<providers::ITimingProvider> timingProvider,
				std::shared_ptr<core::IBeaconSender> beaconSender,
				std::shared_ptr<core::ISessionWatchdog> sessionWatchdog);

			~SessionProxy() override = default;

			std::shared_ptr<openkit::IRootAction> enterAction(const char* actionName) override;

			void identifyUser(const char* userTag) override;

			void reportCrash(const char* errorName, const char* reason, const char* stacktrace) override;

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url) override;

			void end() override;

			bool isFinished();

			void close() override;

			void onChildClosed(std::shared_ptr<IOpenKitObject> childObject) override;

			void onServerConfigurationUpdate(std::shared_ptr<core::configuration::IServerConfiguration> serverConfig) override;

			int64_t splitSessionByTime() override;

			///
			/// Returns the number of top level actions which were made to the current session.
			///
			/// @remarks
			/// Intended to be used by unit tests only.
			///
			int32_t getTopLevelActionCount();

			///
			/// Returns the time when the last top level event was called.
			///
			/// @remarks
			/// Intended to be used by unit tests only.
			///
			int64_t getLastInteractionTime();

			///
			/// Returns the server configuration of this session proxy.
			///
			/// @remarks
			/// Intended to be used by unit tests only.
			///
			std::shared_ptr<core::configuration::IServerConfiguration> getServerConfiguration();

			///
			/// Returns a string describing the object, based on some important fields.
			/// This function is intended for debug printouts.
			///
			/// @return a string describing the object
			///
			const std::string toString() const;

		private:

			SessionProxy(std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<IOpenKitComposite> parent,
				std::shared_ptr<ISessionCreator> sessionCreator,
				std::shared_ptr<providers::ITimingProvider> timingProvider,
				std::shared_ptr<core::IBeaconSender> beaconSender,
				std::shared_ptr<core::ISessionWatchdog> sessionWatchdog);

			void createInitialSession();

			std::shared_ptr<SessionInternals> createSplitSession(std::shared_ptr<core::configuration::IServerConfiguration> updatedServerConfig);

			std::shared_ptr<openkit::ISession> getOrSplitCurrentSessionByEvents();

			///
			/// Creates a new session and adds it to the beacon sender. The top level action count is reset to zero and the
			/// last interaction time is set to the current timestamp.
			///
			/// @par
			/// In case the given <code>initialServerConfig</code> is not null, the new session will be initialized with
			/// this server configuration. The created session however will not be in state
			/// <see cref="ISessionState.IsConfigured">configured</see>, meaning new session requests will be performed for
			/// this session.
			/// 
			/// @par
			/// In case the given <code>updatedServerConfig</code> is not null, the new session will be updated with this
			/// server configuration. The created session will be in state
			/// <see cref="ISessionState.IsConfigured">configured</see>, meaning new session requests will be omitted.
			///
			/// @param initialServerConfig the server configuration with which the session will be initialized. Can be @c nullptr.
			/// @param updatedServerConfig the server configuration with which the session will be updated. Can be @c nullptr.
			///
			std::shared_ptr<SessionInternals> createSession(std::shared_ptr<core::configuration::IServerConfiguration> initialServerConfig,
				std::shared_ptr<core::configuration::IServerConfiguration> updatedServerConfig);

			void updateCurrentSessionIdentifier();

			///
			/// Checks if the maximum number of top level actions is reached and session splitting by events needs to be performed.
			///
			bool isSessionSplitByEventsRequired() const;

			void recordTopLevelEventInteraction();

			void recordTopLevelActionEvent();

			///
			/// Calculates and returns the next point in time when the session is to be split.
			///
			/// The returned time might either be:
			/// - the time when the session expires after the max. session duration elapsed.
			/// - the time when the session expires after being idle.
			/// , depending on what happens earlier.
			///
			int64_t calculateNextSplitTime();

			void reTagCurrentSession();

			///
			/// mutex used for synchronization
			///
			std::recursive_mutex mLockObject;

			///
			/// log message reporter
			///
			std::shared_ptr<openkit::ILogger> mLogger;

			///
			/// parent object of this SessionProxy
			///
			std::shared_ptr<IOpenKitComposite> mParent;

			///
			/// creator for new sessions
			///
			std::shared_ptr<ISessionCreator> mSessionCreator;

			///
			/// provider to obtain the current time
			///
			std::shared_ptr<providers::ITimingProvider> mTimingProvider;

			///
			/// sender of beacon data
			///
			std::shared_ptr<core::IBeaconSender> mBeaconSender;

			///
			/// watchdog to split sessions after idle/max timeout or to close split off session after a grace period
			///
			std::shared_ptr<core::ISessionWatchdog> mSessionWatchdog;

			///
			/// the current session instance
			///
			std::shared_ptr<SessionInternals> mCurrentSession;

			///
			/// Tuple containiner session number/session sequence number of current session.
			/// @remarks
			/// Used for logging purposes, without the need for a strong pointer to session
			///
			std::pair<int32_t, int32_t> mCurrentSessionIdentifier;

			///
			/// holds the number of received calls \see SessionProxy::enterAction(const char*)
			///
			int32_t mTopLevelActionCount;

			///
			/// specifies the timestamp when the last top level event happened
			///
			int64_t mLastInteractionTime;

			///
			/// the server configuration of the first session (will be initialized when the first
			/// session is updated with the server configuration)
			///
			std::shared_ptr<core::configuration::IServerConfiguration> mServerConfiguration;

			///
			/// Indiciator if this SessionProxy is already finished or not.
			///
			bool mIsFinished;

			///
			/// last user tag reported via SessionProxy::identifyUser(const char*)
			/// </summary>
			core::UTF8String mLastUserTag;
		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
