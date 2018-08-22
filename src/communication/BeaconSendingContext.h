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

#ifndef _COMMUNICATION_BEACONSENDINGCONTEXT_H
#define _COMMUNICATION_BEACONSENDINGCONTEXT_H

#include "OpenKit/ILogger.h"
#include "core/util/CountDownLatch.h"
#include "core/util/SynchronizedQueue.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"
#include "configuration/Configuration.h"
#include "protocol/StatusResponse.h"
#include "communication/AbstractBeaconSendingState.h"
#include "core/Session.h"
#include "core/SessionWrapper.h"

#include <atomic>
#include <memory>
#include <chrono>

namespace communication
{
	///
	/// State context for beacon sending states.
	///
	class BeaconSendingContext
	{
	public:
		///
		/// Constructor
		/// @param[in] logger to write traces to
		/// @param[in] httpClientProvider provider for HTTPClient objects
		/// @param[in] timingProvider utility class for timing related stuff
		/// @param[in] configuration general configuration options
		///
		BeaconSendingContext(std::shared_ptr<openkit::ILogger> logger,
			std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
			std::shared_ptr<providers::ITimingProvider> timingProvider,
			std::shared_ptr<configuration::Configuration> configuration);

		///
		/// Constructor
		/// @param[in] logger to write traces to
		/// @param[in] httpClientProvider provider for HTTPClient objects
		/// @param[in] timingProvider utility class for timing related stuff
		/// @param[in] configuration general configuration options
		/// @param[in] initialState the initial state
		///
		BeaconSendingContext(std::shared_ptr<openkit::ILogger> logger, 
			std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
			std::shared_ptr<providers::ITimingProvider> timingProvider,
			std::shared_ptr<configuration::Configuration> configuration,
			std::unique_ptr<communication::AbstractBeaconSendingState> initialState);

		virtual ~BeaconSendingContext() {}

		///
		/// Executes the current state
		///
		void executeCurrentState();

		///
		/// Request shutdown
		///
		virtual void requestShutdown();

		///
		/// Return a flag if shutdown was requested
		/// @returns @c true if shutdown was requested, @c false if not
		///
		virtual bool isShutdownRequested() const;

		///
		/// Blocking method waiting until initialization finished
		/// @return @c true if initialization suceeded, @c false if initialization failed
		///
		bool waitForInit();

		///
		/// Wait until OpenKit has been fully initialized or timeout expired.
		/// If initialization is interrupted (e.g. @ref requestShutdown() was called), then this method also returns.
		/// @param[in] timeoutMillis The maximum number of milliseconds to wait for initialization being completed.
		/// @return @c true if OpenKit is fully initialized, @c false if OpenKit init got interrupted or time to wait expired
		///
		bool waitForInit(int64_t timeoutMillis);

		///
		/// Complete OpenKit initialization
		/// NOTE: This will wake up every caller waiting in the @c #waitForInit() method. 
		/// @param[in] success @c true if OpenKit was successfully initialized, @c false if it was interrupted
		///
		virtual void setInitCompleted(bool success);

		///
		/// Get a boolean indicating whether OpenKit is initialized or not.
		/// @returns @c true  if OpenKit is initialized, @c false otherwise.
		///
		bool isInitialized() const;

		///
		/// Return a flag if the current state of this context is a terminal state
		/// @returns @c true if the current state is a terminal state
		///
		virtual bool isInTerminalState() const;

		///
		/// Returns a flag if capturing is enabled
		/// @returns @c true if capturing is enabled, @c false if capturing is disabled
		///
		virtual bool isCaptureOn() const;

		///
		/// Initialize time synchronisation with cluster time
		/// @param[in] clusterTimeOffset the cluster offset
		/// @param[in] isTimeSyncSupported @c true if time sync is supported, otherwise @c false
		///
		virtual void initializeTimeSync(int64_t clusterTimeOffset, bool isTimeSyncSupported);

		///
		/// Gets a boolean flag indicating whether time sync is supported or not.
		/// @returns @c true if time sync is supported, @c false otherwise.
		///
		virtual bool isTimeSyncSupported() const;

		///
		/// Disables the time sync
		///
		virtual void disableTimeSyncSupport();

		///
		/// Gets a boolean flag indicating whether the time sync has been performed before
		/// @returns @c true if time sync was performed, @c false otherwise
		///
		virtual bool isTimeSynced() const;

		///
		/// Gets the current state.
		/// @returns the current state
		///
		std::shared_ptr<AbstractBeaconSendingState> getCurrentState() const;

		///
		/// Register a state following the current state once the current state finished
		/// @param nextState instance of the  AbstractBeaconSendingState that follows after the current state
		///
		virtual void setNextState(std::shared_ptr<AbstractBeaconSendingState> nextState);

		///
		/// Return the next state for testing purposes
		/// @returns the next state
		///
		std::shared_ptr<AbstractBeaconSendingState> getNextState();

		///
		/// Gets the HTTP client provider.
		/// @return a class responsible for retrieving an instance of @ref protocol::IHTTPClient.
		///
		virtual std::shared_ptr<providers::IHTTPClientProvider> getHTTPClientProvider();

		///
		/// Returns the  HTTPClient created by the current BeaconSendingContext
		/// @returns a shared pointer to the HTTPClient created by the BeaconSendingContext
		///
		virtual std::shared_ptr<protocol::IHTTPClient> getHTTPClient();

		///
		/// Get current timestamp
		/// @returns current timestamp
		///
		virtual int64_t getCurrentTimestamp() const;

		///
		/// Sleep some time (@ref DEFAULT_SLEEP_TIME_MILLISECONDS}
		///
		virtual void sleep();

		///
		/// Sleep for a given amount of time
		/// @param[in] ms number of milliseconds
		///
		virtual void sleep(int64_t ms);

		///
		/// Get timestamp when open sessions were sent last
		/// @returns timestamp of last sending of open session
		///
		virtual int64_t getLastOpenSessionBeaconSendTime() const;

		///
		/// Set timestamp when open sessions were sent last
		/// @param[in] timestamp of last sendinf of open session
		///
		virtual void setLastOpenSessionBeaconSendTime(int64_t timestamp);

		///
		/// Get timestamp when last status check was performed
		/// @returns timestamp of last status check
		///
		int64_t getLastStatusCheckTime() const;

		///
		/// Set timestamp when last status check was performed
		/// @param[in] lastStatusCheckTime timestamp of last status check
		///
		virtual void setLastStatusCheckTime(int64_t lastStatusCheckTime);

		///
		/// Get the send interval for open sessions.
		/// @return the send interval for open sessions
		///
		virtual int64_t getSendInterval() const;

		///
		/// Disable data capturing.
		///
		virtual void disableCapture();

		///
		/// Handle the status response received from the server
		/// Update the current configuration accordingly
		///
		void handleStatusResponse(std::shared_ptr<protocol::StatusResponse> response);

		///
		/// Clears all session data
		///
		void clearAllSessionData();

		///
		/// Get all sessions that are considered new.
		///
		/// The returned list is a snapshot and might change during traversal.
		/// @returns a shallow copy of all new sessions.
		///
		virtual std::vector<std::shared_ptr<core::SessionWrapper>> getAllNewSessions();

		///
		/// Gets all open and configured sessions.
		/// @return a shallow copy of all open sessions.
		///
		virtual std::vector<std::shared_ptr<core::SessionWrapper>> getAllOpenAndConfiguredSessions();

		///
		/// Get a list of all sessions that have been configured and are currently finished
		/// @returns a shallow copy of all finished and configured sessions.
		///
		virtual std::vector<std::shared_ptr<core::SessionWrapper>> getAllFinishedAndConfiguredSessions();

		///
		/// Returns the timestamp when time sync was executed last time.
		/// @returns the timestamp of the last successful time sync
		///
		virtual int64_t getLastTimeSyncTime() const;

		///
		/// Set the timestamp of the last successful time sync
		/// @param[in] lastTimeSyncTime timestamp 
		///
		virtual void setLastTimeSyncTime(int64_t lastTimeSyncTime);

		///
		/// Start a new session.
		/// This add the @c session to the internal container of open sessions.
		/// @param[in] session The new session to start.
		///
		virtual void startSession(std::shared_ptr<core::Session> session);

		///
		/// Finish a session which has been started previously using startSession(std::shared_ptr<core::Session>)
		/// If the session cannot be found in the container storing all open sessions, the parameter is ignored,
		/// otherwise it's removed from the container storing open sessions and added to the finished session container.
		/// @param[in] session The session to finish.
		///
		virtual void finishSession(std::shared_ptr<core::Session> session);

		///
		/// Return the currently used Configuration
		/// @return configuration isntance
		///
		const std::shared_ptr<configuration::Configuration> getConfiguration() const;

		///
		/// Returns the type of state
		/// @returns type of state as defined in AbstractBeaconSendingState
		///
		AbstractBeaconSendingState::StateType getCurrentStateType() const;

		///
		/// Default sleep time in milliseconds (used by @ref sleep()).
		///
		static const std::chrono::milliseconds DEFAULT_SLEEP_TIME_MILLISECONDS;

		///
		/// Search for the session wrapper belonging to the given session
		/// @param[in] session the session to look up in the session wrapper list
		/// @returns the @ref core::SessionWrapper encapsulating the given @c session object
		///
		std::shared_ptr<core::SessionWrapper> findSessionWrapper(std::shared_ptr<core::Session> session);

		///
		/// Remove @ref core::SessionWrapper from the list of all wrappers
		/// @param[in] sessionWrapper
		/// @returns @c true if @c sessionWrapper was found in the session wrapper list and was removed successfully, @c false otherwise
		///
		virtual bool removeSession(std::shared_ptr<core::SessionWrapper> sessionWrapper);

	private:
		/// Logger to write traces to
		std::shared_ptr<openkit::ILogger> mLogger;

		/// instance of AbstractBeaconSendingState with the current state
		std::shared_ptr<AbstractBeaconSendingState> mCurrentState;

		/// instance of AbstractBeaconSendingState with the following state
		std::shared_ptr<AbstractBeaconSendingState> mNextState;

		/// Flag if the current state is a terminal state
		bool mIsInTerminalState;

		/// Atomic shutdown flag
		std::atomic<bool> mShutdown;

		/// Atomic flag for successful initialization
		std::atomic<bool> mInitSucceeded;

		/// The configuration to use
		std::shared_ptr<configuration::Configuration> mConfiguration;

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

		/// flag if time sync is supported
		bool mIsTimeSyncSupported;

		/// timestamp of the last time sync
		int64_t mLastTimeSyncTime;

		/// container storing all session wrappers
		core::util::SynchronizedQueue<std::shared_ptr<core::SessionWrapper>> mSessions;
	};
}
#endif
