/**
 * Copyright 2018-2021 Dynatrace LLC
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
#include "core/configuration/IServerConfiguration.h"
#include "core/objects/SessionInternals.h"
#include "protocol/IAdditionalQueryParameters.h"
#include "protocol/IHTTPClient.h"
#include "protocol/IStatusResponse.h"
#include "protocol/IResponseAttributes.h"
#include "providers/IHTTPClientProvider.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace core
{
	namespace communication
	{
		class IBeaconSendingContext
			: public protocol::IAdditionalQueryParameters
		{
		public:

			virtual ~IBeaconSendingContext() = default;

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
			virtual void disableCaptureAndClear() = 0;

			///
			/// Handle the status response received from the server
			/// Update the current configuration accordingly
			///
			virtual void handleStatusResponse(std::shared_ptr<protocol::IStatusResponse> response) = 0;

			///
			/// Updates the last known response attributes of this context from the given status response if the given IStatusResponse
			/// is successful @see BeaconSendingResponseUtil::isSuccessfulResponse(std::shared_ptr<protocol::IStatusResponse>).
			///
			/// @param statusResponse the status response from which to update the last response attributes.
			///
			/// @return in case the given IStatusResponse was successful the updated response attributes are returned. Otherwise
			/// the current response attributes are returned.
			///
			virtual std::shared_ptr<protocol::IResponseAttributes> updateFrom(std::shared_ptr<protocol::IStatusResponse> statusResponse) = 0;

			///
			/// Returns the last attributes received as response from the server.
			///
			virtual std::shared_ptr<protocol::IResponseAttributes> getLastResponseAttributes() const = 0;

			///
			/// Returns the last known IServerConfiguration.
			///
			virtual std::shared_ptr<core::configuration::IServerConfiguration> getLastServerConfiguration() const = 0;

			///
			/// Get all sessions that were not yet configured.
			///
			/// @par
			/// A session is considered as not configured if it did not receive a server configuration update (either
			/// when receiving a successful for the first new session request or when capturing for the session got
			/// disabled due to an unsuccessful response).
			///
			/// The returned list is a snapshot and might change during traversal.
			///
			/// @returns a shallow copy of all sessions which were not yet configured.
			///
			virtual std::vector<std::shared_ptr<core::objects::SessionInternals>> getAllNotConfiguredSessions() = 0;

			///
			/// Gets all open and configured sessions.
			/// @return a shallow copy of all open sessions.
			///
			virtual std::vector<std::shared_ptr<core::objects::SessionInternals>> getAllOpenAndConfiguredSessions() = 0;

			///
			/// Get a list of all sessions that have been configured and are currently finished
			/// @returns a shallow copy of all finished and configured sessions.
			///
			virtual std::vector<std::shared_ptr<core::objects::SessionInternals>> getAllFinishedAndConfiguredSessions() = 0;

			///
			/// Returns the number of sessions currently known to this context
			///
			virtual size_t getSessionCount() = 0;

			///
			/// Returns the current server ID to be used for creating new sessions.
			///
			virtual int32_t getCurrentServerID() const = 0;

			///
			/// Adds the given session to the internal container of sessions
			///
			/// @param session the new session to add.
			///
			virtual void addSession(std::shared_ptr<core::objects::SessionInternals> session) = 0;

			///
			/// Remove the given session from the sessions known by this context.
			///
			/// @param[in] session the session to be removed.
			/// @returns @c true if @c sessionWrapper was found in the session wrapper list and was removed successfully,
			///   @c false otherwise
			///
			virtual bool removeSession(std::shared_ptr<core::objects::SessionInternals> session) = 0;

			///
			/// Returns the type of state
			/// @returns type of state as defined in IBeaconSendingState
			///
			virtual IBeaconSendingState::StateType getCurrentStateType() const = 0;

		};
	}
}

#endif
