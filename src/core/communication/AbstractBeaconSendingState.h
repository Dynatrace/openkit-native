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

#ifndef _CORE_COMMUNICATION_ABSTRACTBEACONSENDINGSTATE_H
#define _CORE_COMMUNICATION_ABSTRACTBEACONSENDINGSTATE_H

#include "IBeaconSendingState.h"

#include <memory>

namespace core
{
	namespace communication
	{
		class IBeaconSendingContext;

		/// forward declaration of class BeaconSendingContext
		class BeaconSendingContext;

		///
		/// Abstract class for all beacon sending states
		///
		class AbstractBeaconSendingState
			: public IBeaconSendingState
		{
		public:
			///
			/// Constructor
			///
			explicit AbstractBeaconSendingState(StateType type);

			///
			/// Destructor
			///
			~AbstractBeaconSendingState() override {}


			///
			/// Execute the current state.
			///
			/// In case shutdown was requested, a state transition is performed by this method to the @ref AbstractBeaconSendingState returned by @ref AbstractBeaconSendingState::getShutdownState().
			/// @param[in] context the @ref BeaconSendingContext that takes care of state transitions
			///
			void execute(IBeaconSendingContext& context) override;

			///
			/// Get an instance of the {@ref AbstractBeaconSendingState} to which a transition is made upon shutdown request.
			/// @returns the follow-up state taking care of the shutdown
			///
			std::shared_ptr<IBeaconSendingState> getShutdownState() override = 0;

			///
			/// Returns @c true if this state is a terminal state, @c false otherwise.
			/// @returns @c true if this state is a terminal state, @c false otherwise.
			///
			bool isTerminalState() const override;

			///
			/// Returns the enum for the state
			/// @returns the enum for the state
			///
			StateType getStateType() const override;

			///
			/// Returns the name of the state
			/// @returns the name of the state
			///
			const char* getStateName() const override = 0;

		protected:
			///
			/// Execute the state - real state execution - has to overriden by subclas
			/// @param context the @see BeaconSendingContext that takes care of state transitions
			///
			virtual void doExecute(IBeaconSendingContext& context) = 0;

		private:
			/// state type
			StateType mStateType;
		};
	}
}
#endif