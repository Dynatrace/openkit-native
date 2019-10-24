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

#ifndef _CORE_COMMUNICATION_BEACONSENDINGCONTEXT_H
#define _CORE_COMMUNICATION_BEACONSENDINGCONTEXT_H

#include "IBeaconSendingContext.h"
#include "IBeaconSendingState.h"
#include "OpenKit/ILogger.h"
#include "core/SessionWrapper.h"
#include "core/configuration/Configuration.h"
#include "core/objects/SessionInternals.h"
#include "core/util/CountDownLatch.h"
#include "core/util/SynchronizedQueue.h"
#include "protocol/IStatusResponse.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <chrono>

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
			/// @param[in] httpClientProvider provider for HTTPClient objects
			/// @param[in] timingProvider utility class for timing related stuff
			/// @param[in] configuration general configuration options
			///
			BeaconSendingContext(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
				std::shared_ptr<providers::ITimingProvider> timingProvider,
				std::shared_ptr<core::configuration::Configuration> configuration
			);

			///
			/// Constructor
			/// @param[in] logger to write traces to
			/// @param[in] httpClientProvider provider for HTTPClient objects
			/// @param[in] timingProvider utility class for timing related stuff
			/// @param[in] configuration general configuration options
			/// @param[in] initialState the initial state
			///
			BeaconSendingContext(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
				std::shared_ptr<providers::ITimingProvider> timingProvider,
				std::shared_ptr<core::configuration::Configuration> configuration,
				std::unique_ptr<IBeaconSendingState> initialState
			);

			~BeaconSendingContext() override = default;

			///
			/// Executes the current state
			///
			void executeCurrentState() override ;

			///
			/// Request shutdown
			///
			void requestShutdown() override;

			///
			/// Return a flag if shutdown was requested
			/// @returns @c true if shutdown was requested, @c false if not
			///
			bool isShutdownRequested() const override;

			///
			/// Blocking method waiting until initialization finished
			/// @return @c true if initialization succeeded, @c false if initialization failed
			///
			bool waitForInit() override ;

			///
			/// Wait until OpenKit has been fully initialized or timeout expired.
			/// If initialization is interrupted (e.g. @ref requestShutdown() was called), then this method also returns.
			/// @param[in] timeoutMillis The maximum number of milliseconds to wait for initialization being completed.
			/// @return @c true if OpenKit is fully initialized, @c false if OpenKit init got interrupted or time to wait expired
			///
			bool waitForInit(int64_t timeoutMillis) override ;

			///
			/// Complete OpenKit initialization
			/// NOTE: This will wake up every caller waiting in the @c #waitForInit() method.
			/// @param[in] success @c true if OpenKit was successfully initialized, @c false if it was interrupted
			///
			void setInitCompleted(bool success) override;

			///
			/// Get a boolean indicating whether OpenKit is initialized or not.
			/// @returns @c true  if OpenKit is initialized, @c false otherwise.
			///
			bool isInitialized() const override;

			///
			/// Return a flag if the current state of this context is a terminal state
			/// @returns @c true if the current state is a terminal state
			///
			bool isInTerminalState() const override;

			///
			/// Returns a flag if capturing is enabled
			/// @returns @c true if capturing is enabled, @c false if capturing is disabled
			///
			bool isCaptureOn() const override;

			///
			/// Gets the current state.
			/// @returns the current state
			///
			std::shared_ptr<IBeaconSendingState> getCurrentState() const override;

			///
			/// Register a state following the current state once the current state finished
			/// @param nextState instance of the  IBeaconSendingState that follows after the current state
			///
			void setNextState(std::shared_ptr<IBeaconSendingState> nextState) override;

			///
			/// Return the next state for testing purposes
			/// @returns the next state
			///
			std::shared_ptr<IBeaconSendingState> getNextState() override;

			///
			/// Gets the HTTP client provider.
			/// @return a class responsible for retrieving an instance of @ref protocol::IHTTPClient.
			///
			std::shared_ptr<providers::IHTTPClientProvider> getHTTPClientProvider() override;

			///
			/// Returns the  HTTPClient created by the current BeaconSendingContext
			/// @returns a shared pointer to the HTTPClient created by the BeaconSendingContext
			///
			std::shared_ptr<protocol::IHTTPClient> getHTTPClient() override;

			///
			/// Get current timestamp
			/// @returns current timestamp
			///
			int64_t getCurrentTimestamp() const override;

			///
			/// Sleep some time (@ref DEFAULT_SLEEP_TIME_MILLISECONDS}
			///
			void sleep() override;

			///
			/// Sleep for a given amount of time
			/// @param[in] ms number of milliseconds
			///
			void sleep(int64_t ms) override;

			///
			/// Get timestamp when open sessions were sent last
			/// @returns timestamp of last sending of open session
			///
			int64_t getLastOpenSessionBeaconSendTime() const override;

			///
			/// Set timestamp when open sessions were sent last
			/// @param[in] timestamp of last sendinf of open session
			///
			void setLastOpenSessionBeaconSendTime(int64_t timestamp) override;

			///
			/// Get timestamp when last status check was performed
			/// @returns timestamp of last status check
			///
			int64_t getLastStatusCheckTime() const override;

			///
			/// Set timestamp when last status check was performed
			/// @param[in] lastStatusCheckTime timestamp of last status check
			///
			void setLastStatusCheckTime(int64_t lastStatusCheckTime) override;

			///
			/// Get the send interval for open sessions.
			/// @return the send interval for open sessions
			///
			int64_t getSendInterval() const override;

			///
			/// Disable data capturing.
			///
			void disableCapture() override;

			///
			/// Handle the status response received from the server
			/// Update the current configuration accordingly
			///
			void handleStatusResponse(std::shared_ptr<protocol::IStatusResponse> response) override;

			///
			/// Clears all session data
			///
			void clearAllSessionData() override;

			///
			/// Get all sessions that are considered new.
			///
			/// The returned list is a snapshot and might change during traversal.
			/// @returns a shallow copy of all new sessions.
			///
			std::vector<std::shared_ptr<core::SessionWrapper>> getAllNewSessions() override;

			///
			/// Gets all open and configured sessions.
			/// @return a shallow copy of all open sessions.
			///
			std::vector<std::shared_ptr<core::SessionWrapper>> getAllOpenAndConfiguredSessions() override;

			///
			/// Get a list of all sessions that have been configured and are currently finished
			/// @returns a shallow copy of all finished and configured sessions.
			///
			std::vector<std::shared_ptr<core::SessionWrapper>> getAllFinishedAndConfiguredSessions() override;

			///
			/// Start a new session.
			/// This add the @c session to the internal container of open sessions.
			/// @param[in] session The new session to start.
			///
			void startSession(std::shared_ptr<core::objects::SessionInternals> session) override;

			///
			/// Finish a session which has been started previously using startSession(std::shared_ptr<core::objects::Session>)
			/// If the session cannot be found in the container storing all open sessions, the parameter is ignored,
			/// otherwise it's removed from the container storing open sessions and added to the finished session container.
			/// @param[in] session The session to finish.
			///
			void finishSession(std::shared_ptr<core::objects::SessionInternals> session) override;

			///
			/// Returns the type of state
			/// @returns type of state as defined inIBeaconSendingState
			///
			IBeaconSendingState::StateType getCurrentStateType() const override;

			///
			/// Default sleep time in milliseconds (used by @ref sleep()).
			///
			static const std::chrono::milliseconds DEFAULT_SLEEP_TIME_MILLISECONDS;

			///
			/// Search for the session wrapper belonging to the given session
			/// @param[in] session the session to look up in the session wrapper list
			/// @returns the @ref core::SessionWrapper encapsulating the given @c session object
			///
			std::shared_ptr<core::SessionWrapper> findSessionWrapper(std::shared_ptr<core::objects::SessionInternals> session);

			///
			/// Remove @ref core::SessionWrapper from the list of all wrappers
			/// @param[in] sessionWrapper
			/// @returns @c true if @c sessionWrapper was found in the session wrapper list and was removed successfully, @c false otherwise
			///
			bool removeSession(std::shared_ptr<core::SessionWrapper> sessionWrapper) override;

		private:
			/// Logger to write traces to
			std::shared_ptr<openkit::ILogger> mLogger;

			/// instance of IBeaconSendingState with the current state
			std::shared_ptr<IBeaconSendingState> mCurrentState;

			/// instance of IBeaconSendingState with the following state
			std::shared_ptr<IBeaconSendingState> mNextState;

			/// Boolean indicating shutdown flag.
			bool mShutdown;

			/// mutex used for synchronisation access to mShutdown
			mutable std::mutex mShutdownMutex;

			/// condition variable used to wait on when calling sleep.
			std::condition_variable mSleepConditionVariable;

			/// Atomic flag for successful initialization
			std::atomic<bool> mInitSucceeded;

			/// The configuration to use
			std::shared_ptr<core::configuration::Configuration> mConfiguration;

			/// IHTTPClientProvider responsible for creating instances of HTTPClient
			std::shared_ptr<providers::IHTTPClientProvider> mHTTPClientProvider;

			/// TimingPRovider used by the BeaconSendingContext
			std::shared_ptr<providers::ITimingProvider> mTimingProvider;

			/// time of the last status check
			int64_t mLastStatusCheckTime;

			/// time when open sessions were last sent
			int64_t mLastOpenSessionBeaconSendTime;

			/// countdown latch used for wait-on-initialization
			core::util::CountDownLatch mInitCountdownLatch;

			/// container storing all session wrappers
			core::util::SynchronizedQueue<std::shared_ptr<core::SessionWrapper>> mSessions;
		};
	}
}
#endif
