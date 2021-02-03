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

#ifndef _CORE_COMMUNICATION_IBEACONSENDINGSTATE_H
#define _CORE_COMMUNICATION_IBEACONSENDINGSTATE_H

#include <memory>

namespace core
{
	namespace communication
	{
		class IBeaconSendingContext;

		class IBeaconSendingState
		{
		public:
			/// This is pseudo runtime type information used in the tests
			enum class StateType
			{
				BEACON_SENDING_INIT_STATE,
				BEACON_SENDING_CAPTURE_ON_STATE,
				BEACON_SENDING_CAPTURE_OFF_STATE,
				BEACON_SENDING_FLUSH_SESSIONS_STATE,
				BEACON_SENDING_TERMINAL_STATE,
				BEACON_SENDING_COUNT
			};

			virtual ~IBeaconSendingState() = default;

			///
			/// Execute the current state.
			///
			/// In case shutdown was requested, a state transition is performed by this method to the @ref AbstractBeaconSendingState returned by @ref AbstractBeaconSendingState::getShutdownState().
			/// @param[in] context the @ref BeaconSendingContext that takes care of state transitions
			///
			virtual void execute(IBeaconSendingContext& context) = 0;

			///
			/// Get an instance of the {@ref AbstractBeaconSendingState} to which a transition is made upon shutdown request.
			/// @returns the follow-up state taking care of the shutdown
			///
			virtual std::shared_ptr<IBeaconSendingState> getShutdownState() = 0;

			///
			/// Returns @c true if this state is a terminal state, @c false otherwise.
			/// @returns @c true if this state is a terminal state, @c false otherwise.
			///
			virtual bool isTerminalState() const = 0;

			///
			/// Returns the enum for the state
			/// @returns the enum for the state
			///
			virtual StateType getStateType() const = 0;

			///
			/// Returns the name of the state
			/// @returns the name of the state
			///
			virtual const char* getStateName() const = 0;
		};
	}
}

#endif
