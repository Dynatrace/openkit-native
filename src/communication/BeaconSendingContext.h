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

#include <atomic>
#include <memory>

#include "core/util/CountDownLatch.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"
#include "configuration/Configuration.h"
#include "protocol/StatusResponse.h"

namespace communication {

	//forward declaration to keep the AbstractBeaconSendingState header out of this header
	class AbstractBeaconSendingState;

	///
	/// State context for beacon sending states.
	///
	class BeaconSendingContext
	{
	public:
		///
		/// Constructor
		/// @param[in] httpClientProvider provider for HTTPClient objects
		/// @param[in] timingProvider utility class for timing related stuff
		/// @param[in] configuration general configuration options
		///
		BeaconSendingContext(std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
			std::shared_ptr<providers::ITimingProvider> timingProvider,
			std::shared_ptr<configuration::Configuration> configuration);

		///
		/// Register a state following the current state once the current state finished
		/// @param nextState instance of the @s AbstractBeaconSendingState that follows after the current state
		///
		void setNextState(std::unique_ptr<AbstractBeaconSendingState> nextState);

		///
		/// Return a flag if the current state of this context is a terminal state
		/// @returns @s true if the current state is a terminal state
		///
		bool isInTerminalState() const;

		///
		/// Executes the current state
		///
		void executeCurrentState();

		///
		/// Request shutdown
		///
		void requestShutdown();

		///
		/// Return a flag if shutdown was requested
		/// @returns @s true if shutdown was requested, @s false if not
		///
		bool isShutdownRequested() const;

		///
		/// Return the currently used @s Configuration
		/// @return configuration isntance
		///
		const std::shared_ptr<configuration::Configuration> getConfiguration() const;

		///
		/// Returns the HTTPClient created by the current BeaconSendingContext
		/// @returns a shared pointer to the HTTTP client created by the BeaconSendingContext
		///
		std::unique_ptr<protocol::HTTPClient> getHTTPClient();

		///
		/// Handle the status response received from the server
		/// Update the current configuration accordingly
		///
		void handleStatusResponse(std::unique_ptr<protocol::StatusResponse> response);

		///
		/// Clears all session data
		///
		void clearAllSessionData();

		///
		/// Returns a flag if capturing is enabled
		/// @returns @s true if capturing is enabled, @s false if capturing is disabled
		///
		bool isCaptureOn() const;

		///
		/// Complete OpenKit initialisation
		/// NOTE: This will wake up every caller waiting in the {@link #waitForInit()} method. 
		/// @param[in] success @s true if OpenKit was successfully initialized, @s false if it was interrupted
		///
		void setInitCompleted(bool success);

		///
		/// Get a boolean indicating whether OpenKit is initialized or not.
		/// @returns @c true  if OpenKit is initialized, @c false otherwise.
		///
		bool isInitialised() const;

		///
		/// Sleep for a given amount of time
		/// @param[in] ms number of milliseconds
		///
		void sleep(uint64_t ms);

		///
		/// Get current timestamp
		/// @returns current timestamp
		///
		uint64_t getCurrentTimestamp() const;

		///
		/// Get timestamp when last status check was performed
		/// @returns timestamp of last status check
		///
		uint64_t getLastStatusCheckTime() const;

		///
		/// Set timestamp when last status check was performed
		/// @param[in] lastStatusCheckTime timestamp of last status check
		///
		void setLastStatusCheckTime(uint64_t lastStatusCheckTime);

		///
		/// Get timestamp when open sessions were sent last
		/// @returns timestamp timestamp of last sending of open session
		///
		uint64_t getLastOpenSessionBeaconSendTime();

		///
		/// Set timestamp when open sessions were sent last
		/// @param[in] timestamp  timestamp of last sendinf of open session
		///
		void setLastOpenSessionBeaconSendTime(uint64_t timestamp);

	private:
		/// instance of @s AbstractBeaconSendingState with the current state
		std::unique_ptr<AbstractBeaconSendingState> mCurrentState;

		/// Flag if the current state is a terminal state
		bool mIsInTerminalState;

		/// Atomic shutdown flag
		std::atomic<bool> mShutdown;

		/// Atomic flag for successful initialisation
		std::atomic<bool> mInitSuceeded;

		/// The configuration to use
		std::shared_ptr<configuration::Configuration> mConfiguration;

		/// IHTTPClientProvider responsible for creating instances of HTTPClient
		std::shared_ptr<providers::IHTTPClientProvider> mHTTPClientProvider;

		/// TimingPRovider used by the BeaconSendingContext
		std::shared_ptr<providers::ITimingProvider> mTimingProvider;

		/// time of the last status check
		uint64_t mLastStatusCheckTime;

		/// time when open sessions were last sent
		uint64_t mLastOpenSessionBeaconSendTime;

		/// countdown latch used for wait-on-initialisation
		core::util::CountDownLatch mInitCountdownLatch;
	};
}
#endif