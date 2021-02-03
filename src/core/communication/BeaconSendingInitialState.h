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

#ifndef _CORE_COMMUNICATION_BEACONSENDINGINITIALSTATE_H
#define _CORE_COMMUNICATION_BEACONSENDINGINITIALSTATE_H

#include "AbstractBeaconSendingState.h"
#include "IBeaconSendingContext.h"
#include "protocol/IStatusResponse.h"

#include <array>
#include <chrono>
#include <memory>

namespace core
{
	namespace communication
	{
		///
		/// Initial state for beacon sending.
		/// The initial state is used to retrieve the configuration from the server and update the configuration.
		///
		/// Transition to:
		///   - @ref BeaconSendingTerminalState upon shutdown request
		///   - @ref BeaconSendingCaptureOnState if initial status request succeeded and capturing is enabled.
		///   - @ref BeaconSendingCaptureOffState if initial status request succeeded and capturing is disabled.
		///
		class BeaconSendingInitialState
			: public AbstractBeaconSendingState
		{
		public:
			///
			/// Constructor
			///
			BeaconSendingInitialState();

			///
			/// Destructor
			///
			~BeaconSendingInitialState() override = default;

			void doExecute(IBeaconSendingContext& context) override;

			std::shared_ptr<IBeaconSendingState> getShutdownState() override;

			const char* getStateName() const override;
			
			///
			/// Returns the initial delay which is later on doubled between one unsuccessful attempt and the next retry
			///
			static const std::chrono::milliseconds& getInitialRetrySleepTimeMilliseconds();

			///
			/// Returns times to use as delay between consecutive re-executions of this state, when no state transition is performed
			///
			static const std::array<std::chrono::milliseconds, 5>& getReInitDelayMilliseconds();

		private:

			///
			/// Execute status requests, until a successful response was received or shutdown was requested.
			/// @param context The state's context
			/// @return The last received status response, which might be erroneous if shutdown has been requested.
			///
			std::shared_ptr<protocol::IStatusResponse> executeStatusRequest(IBeaconSendingContext& context);

			///
			/// Index to re-initialize delays
			///
			uint32_t mReinitializeDelayIndex;
		};
	}
}

#endif
