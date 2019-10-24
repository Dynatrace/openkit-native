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

#ifndef _CORE_COMMUNICATION_IBEACONSENDINGCONTEXT_H
#define _CORE_COMMUNICATION_IBEACONSENDINGCONTEXT_H

#include "IBeaconSendingState.h"
#include "core/SessionWrapper.h"
#include "core/objects/Session.h"
#include "protocol/IHTTPClient.h"
#include "protocol/IStatusResponse.h"
#include "providers/IHTTPClientProvider.h"

#include <cstdint>
#include <memory>

namespace core
{
	namespace communication
	{
		class IBeaconSendingContext
		{
		public:

			virtual ~IBeaconSendingContext() {}

			///
			/// Executes the current state
			///
			virtual void executeCurrentState() = 0;

			///
			/// Request shutdown
			///
			virtual void requestShutdown() = 0;

			///
			/// Return a flag if shutdown was requested
			/// @returns @c true if shutdown was requested, @c false if not
			///
			virtual bool isShutdownRequested() const = 0;

			///
			/// Blocking method waiting until initialization finished
			/// @return @c true if initialization succeeded, @c false if initialization failed
			///
			virtual bool waitForInit() = 0;

			///
			/// Wait until OpenKit has been fully initialized or timeout expired.
			/// If initialization is interrupted (e.g. @ref requestShutdown() was called), then this method also returns.
			/// @param[in] timeoutMillis The maximum number of milliseconds to wait for initialization being completed.
			/// @return @c true if OpenKit is fully initialized, @c false if OpenKit init got interrupted or time to wait expired
			///
			virtual bool waitForInit(int64_t timeoutMillis) = 0;

			///
			/// Complete OpenKit initialization
			/// NOTE: This will wake up every caller waiting in the @c #waitForInit() method.
			/// @param[in] success @c true if OpenKit was successfully initialized, @c false if it was interrupted
			///
			virtual void setInitCompleted(bool success) = 0;

			///
			/// Get a boolean indicating whether OpenKit is initialized or not.
			/// @returns @c true  if OpenKit is initialized, @c false otherwise.
			///
			virtual bool isInitialized() const = 0;

			///
			/// Return a flag if the current state of this context is a terminal state
			/// @returns @c true if the current state is a terminal state
			///
			virtual bool isInTerminalState() const = 0;

			///
			/// Returns a flag if capturing is enabled
			/// @returns @c true if capturing is enabled, @c false if capturing is disabled
			///
			virtual bool isCaptureOn() const = 0;

			///
			/// Gets the current state.
			/// @returns the current state
			///
			virtual std::shared_ptr<IBeaconSendingState> getCurrentState() const = 0;

			///
			/// Register a state following the current state once the current state finished
			/// @param nextState instance of the  IBeaconSendingState that follows after the current state
			///
			virtual void setNextState(std::shared_ptr<IBeaconSendingState> nextState) = 0;

			///
			/// Return the next state for testing purposes
			/// @returns the next state
			///
			virtual std::shared_ptr<IBeaconSendingState> getNextState() = 0;

			///
			/// Gets the HTTP client provider.
			/// @return a class responsible for retrieving an instance of @ref protocol::IHTTPClient.
			///
			virtual std::shared_ptr<providers::IHTTPClientProvider> getHTTPClientProvider() = 0;

			///
			/// Returns the  HTTPClient created by the current BeaconSendingContext
			/// @returns a shared pointer to the HTTPClient created by the BeaconSendingContext
			///
			virtual std::shared_ptr<protocol::IHTTPClient> getHTTPClient() = 0;

			///
			/// Get current timestamp
			/// @returns current timestamp
			///
			virtual int64_t getCurrentTimestamp() const = 0;

			///
			/// Sleep some time (@ref DEFAULT_SLEEP_TIME_MILLISECONDS}
			///
			virtual void sleep() = 0;

			///
			/// Sleep for a given amount of time
			/// @param[in] ms number of milliseconds
			///
			virtual void sleep(int64_t ms) = 0;

			///
			/// Get timestamp when open sessions were sent last
			/// @returns timestamp of last sending of open session
			///
			virtual int64_t getLastOpenSessionBeaconSendTime() const = 0;

			///
			/// Set timestamp when open sessions were sent last
			/// @param[in] timestamp of last sendinf of open session
			///
			virtual void setLastOpenSessionBeaconSendTime(int64_t timestamp) = 0;

			///
			/// Get timestamp when last status check was performed
			/// @returns timestamp of last status check
			///
			virtual int64_t getLastStatusCheckTime() const = 0;

			///
			/// Set timestamp when last status check was performed
			/// @param[in] lastStatusCheckTime timestamp of last status check
			///
			virtual void setLastStatusCheckTime(int64_t lastStatusCheckTime) = 0;

			///
			/// Get the send interval for open sessions.
			/// @return the send interval for open sessions
			///
			virtual int64_t getSendInterval() const = 0;

			///
			/// Disable data capturing.
			///
			virtual void disableCapture() = 0;

			///
			/// Handle the status response received from the server
			/// Update the current configuration accordingly
			///
			virtual void handleStatusResponse(std::shared_ptr<protocol::IStatusResponse> response) = 0;

			///
			/// Clears all session data
			///
			virtual void clearAllSessionData() = 0;

			///
			/// Get all sessions that are considered new.
			///
			/// The returned list is a snapshot and might change during traversal.
			/// @returns a shallow copy of all new sessions.
			///
			virtual std::vector<std::shared_ptr<core::SessionWrapper>> getAllNewSessions() = 0;

			///
			/// Gets all open and configured sessions.
			/// @return a shallow copy of all open sessions.
			///
			virtual std::vector<std::shared_ptr<core::SessionWrapper>> getAllOpenAndConfiguredSessions() = 0;

			///
			/// Get a list of all sessions that have been configured and are currently finished
			/// @returns a shallow copy of all finished and configured sessions.
			///
			virtual std::vector<std::shared_ptr<core::SessionWrapper>> getAllFinishedAndConfiguredSessions() = 0;

			///
			/// Start a new session.
			/// This add the @c session to the internal container of open sessions.
			/// @param[in] session The new session to start.
			///
			virtual void startSession(std::shared_ptr<core::objects::Session> session) = 0;

			///
			/// Finish a session which has been started previously using startSession(std::shared_ptr<core::objects::Session>)
			/// If the session cannot be found in the container storing all open sessions, the parameter is ignored,
			/// otherwise it's removed from the container storing open sessions and added to the finished session container.
			/// @param[in] session The session to finish.
			///
			virtual void finishSession(std::shared_ptr<core::objects::Session> session) = 0;

			///
			/// Returns the type of state
			/// @returns type of state as defined in IBeaconSendingState
			///
			virtual IBeaconSendingState::StateType getCurrentStateType() const = 0;

			///
			/// Remove @ref core::SessionWrapper from the list of all wrappers
			/// @param[in] sessionWrapper
			/// @returns @c true if @c sessionWrapper was found in the session wrapper list and was removed successfully, @c false otherwise
			///
			virtual bool removeSession(std::shared_ptr<core::SessionWrapper> sessionWrapper) = 0;
		};
	}
}

#endif
