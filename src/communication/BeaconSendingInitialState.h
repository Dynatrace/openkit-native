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

#ifndef _COMMUNICATION_BEACONSENDINGINITIALSTATE_H
#define _COMMUNICATION_BEACONSENDINGINITIALSTATE_H

#include "communication/AbstractBeaconSendingState.h"
#include "protocol/StatusResponse.h"

#include <vector>
#include <chrono>
#include <memory>

namespace communication
{
	///
	/// Initial state for beacon sending.
	/// The initial state is used to retrieve the configuration from the server and update the configuration.
	///
	/// Transition to:
	///   - @ref BeaconSendingTerminalState upon shutdown request
	///   - @ref BeaconSendingTimeSyncState if initial status request succeeded
	///
	class BeaconSendingInitialState : public AbstractBeaconSendingState
	{
	public:
		///
		/// Constructor
		///
		BeaconSendingInitialState();

		///
		/// Destructor
		///
		virtual ~BeaconSendingInitialState() {}

		virtual void doExecute(BeaconSendingContext& context) override;

		virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState() override;

		virtual const char* getStateName() const override;

		/// The initial delay which is later on doubled between one unsuccessful attempt and the next retry
		static const std::chrono::milliseconds INITIAL_RETRY_SLEEP_TIME_MILLISECONDS;

		/// Times to use as delay between consecutive re-executions of this state, when no state transition is performed
		static const std::vector<std::chrono::milliseconds> REINIT_DELAY_MILLISECONDS;

	private:

		///
		/// Execute status requests, until a successful response was received or shutdown was requested.
		/// @param context The state's context
		/// @return The last received status response, which might be erroneous if shutdown has been requested.
		///
		std::shared_ptr<protocol::StatusResponse> executeStatusRequest(BeaconSendingContext& context);

		///
		/// Index to re-initialize delays
		///
		uint32_t mReinitializeDelayIndex;
	};
}

#endif
