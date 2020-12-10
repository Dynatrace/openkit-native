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

#ifndef _CORE_COMMUNICATION_BEACONSENDINGCONTEXT_H
#define _CORE_COMMUNICATION_BEACONSENDINGCONTEXT_H

#include "OpenKit/ILogger.h"
#include "IBeaconSendingContext.h"
#include "IBeaconSendingState.h"
#include "core/configuration/IHTTPClientConfiguration.h"
#include "core/objects/SessionInternals.h"
#include "core/util/CountDownLatch.h"
#include "core/util/IInterruptibleThreadSuspender.h"
#include "core/util/SynchronizedQueue.h"
#include "protocol/IStatusResponse.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <cstdint>

namespace core
{
	namespace communication
	{
		///
		/// State context for beacon sending states.
		///
		class BeaconSendingContext
			: public IBeaconSendingContext
		{
		public:
			///
			/// Constructor
			/// @param[in] logger to write traces to
			/// @param[in] httpClientConfiguration HTTP related configuration details
			/// @param[in] httpClientProvider provider for HTTPClient objects
			/// @param[in] timingProvider utility class for timing related stuff
			///
			BeaconSendingContext(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<core::configuration::IHTTPClientConfiguration> httpClientConfiguration,
				std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
				std::shared_ptr<providers::ITimingProvider> timingProvider,
				std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender
			);

			///
			/// Constructor
			/// @param[in] logger to write traces to
			/// @param[in] httpClientConfiguration HTTP related configuration details
			/// @param[in] httpClientProvider provider for HTTPClient objects
			/// @param[in] timingProvider utility class for timing related stuff
			/// @param[in] initialState the initial state
			///
			BeaconSendingContext(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<core::configuration::IHTTPClientConfiguration> httpClientConfiguration,
				std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
				std::shared_ptr<providers::ITimingProvider> timingProvider,
				std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender,
				std::unique_ptr<IBeaconSendingState> initialState
			);

			~BeaconSendingContext() override = default;

			///
			/// Default sleep time in milliseconds (used by @ref sleep()).
			///
			static const std::chrono::milliseconds& getDefaultSleepTime();

			void executeCurrentState() override;

			void requestShutdown() override;

			bool isShutdownRequested() const override;

			bool waitForInit() override ;

			bool waitForInit(int64_t timeoutMillis) override ;

			void setInitCompleted(bool success) override;

			bool isInitialized() const override;

			bool isInTerminalState() const override;

			bool isCaptureOn() const override;

			std::shared_ptr<IBeaconSendingState> getCurrentState() const override;

			void setNextState(std::shared_ptr<IBeaconSendingState> nextState) override;

			std::shared_ptr<IBeaconSendingState> getNextState() override;

			std::shared_ptr<providers::IHTTPClientProvider> getHTTPClientProvider() override;

			std::shared_ptr<protocol::IHTTPClient> getHTTPClient() override;

			int64_t getCurrentTimestamp() const override;

			void sleep() override;

			void sleep(int64_t ms) override;

			int64_t getLastOpenSessionBeaconSendTime() const override;

			void setLastOpenSessionBeaconSendTime(int64_t timestamp) override;

			int64_t getLastStatusCheckTime() const override;

			void setLastStatusCheckTime(int64_t lastStatusCheckTime) override;

			int64_t getSendInterval() const override;

			void disableCaptureAndClear() override;

			void handleStatusResponse(std::shared_ptr<protocol::IStatusResponse> response) override;

			std::shared_ptr<protocol::IResponseAttributes> updateFrom(std::shared_ptr<protocol::IStatusResponse> statusResponse) override;

			std::shared_ptr<protocol::IResponseAttributes> getLastResponseAttributes() const override;

			std::shared_ptr<core::configuration::IServerConfiguration> getLastServerConfiguration() const override;

			std::vector<std::shared_ptr<core::objects::SessionInternals>> getAllNotConfiguredSessions() override;

			std::vector<std::shared_ptr<core::objects::SessionInternals>> getAllOpenAndConfiguredSessions() override;

			std::vector<std::shared_ptr<core::objects::SessionInternals>> getAllFinishedAndConfiguredSessions() override;

			size_t getSessionCount() override;

			int32_t getCurrentServerID() const override;

			void addSession(std::shared_ptr<core::objects::SessionInternals> session) override;

			bool removeSession(std::shared_ptr<core::objects::SessionInternals> session) override;

			IBeaconSendingState::StateType getCurrentStateType() const override;

			int64_t getConfigurationTimestamp() const override;

			///
			/// Ensure that the application id coming with the response matches the one that was configured for OpenKit.
			///
			/// Mismatch check prevents a rare Jetty bug, where responses might be dispatched to the wrong receiver.
			/// 
			/// @param The last response attributes received from Dynatrace/AppMon.
			/// @returns false if application id is matching, true if a mismatch occurred.
			/// 
			bool isApplicationIdMismatch(std::shared_ptr<protocol::IResponseAttributes> lastResponseAttributes) const;

		private:

			///
			/// synchronization object for updating and reading server configuration and last response attributes
			///
			mutable std::mutex mLockObject;

			///
			/// Disable data capturing.
			///
			void disableCapture();

			///
			/// Clears all session data
			///
			void clearAllSessionData();

			/// Logger to write traces to
			std::shared_ptr<openkit::ILogger> mLogger;

			/// instance of IBeaconSendingState with the current state
			std::shared_ptr<IBeaconSendingState> mCurrentState;

			/// instance of IBeaconSendingState with the following state
			std::shared_ptr<IBeaconSendingState> mNextState;

			/// Boolean indicating shutdown flag.
			std::atomic<bool> mShutdown;

			/// Atomic flag for successful initialization
			std::atomic<bool> mInitSucceeded;

			///
			/// Configuration storing last valid server side configuration.
			///
			/// @remarks
			/// This field is initialized in the constructor and must only be modified within the context of the
			/// beacon sending thread.
			///
			std::shared_ptr<core::configuration::IServerConfiguration> mServerConfiguration;

			///
			/// Configuration storing the last valid HTTP client configuration, independent of a session.
			///
			/// @remarks
			/// This field is initialized in the constructor and must only be modified within the context of the
			/// beacon sending thread.
			///
			std::shared_ptr<core::configuration::IHTTPClientConfiguration> mHTTPClientConfiguration;

			/// IHTTPClientProvider responsible for creating instances of HTTPClient
			std::shared_ptr<providers::IHTTPClientProvider> mHTTPClientProvider;

			/// TimingPRovider used by the BeaconSendingContext
			std::shared_ptr<providers::ITimingProvider> mTimingProvider;

			/// time of the last status check
			int64_t mLastStatusCheckTime;

			/// time when open sessions were last sent
			int64_t mLastOpenSessionBeaconSendTime;

			///
			/// This property will be subsequently updated from every successful server response (e.g. from session
			/// requests) by merging the received attributes.
			///
			/// @remarks
			/// Modification of this field must only happen within the context of the BeaconSending thread.
			///
			std::shared_ptr<protocol::IResponseAttributes> mLastResponseAttributes;

			/// countdown latch used for wait-on-initialization
			core::util::CountDownLatch mInitCountdownLatch;

			///
			/// instance for suspending the thread for a certain time span.
			///
			std::shared_ptr<core::util::IInterruptibleThreadSuspender> mThreadSuspender;

			/// container storing all session wrappers
			core::util::SynchronizedQueue<std::shared_ptr<core::objects::SessionInternals>> mSessions;
		};
	}
}
#endif
