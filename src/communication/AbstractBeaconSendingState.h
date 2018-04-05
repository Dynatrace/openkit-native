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

#ifndef _COMMUNICATION_ABSTRACTBEACONSENDINGSTATE_H
#define _COMMUNICATION_ABSTRACTBEACONSENDINGSTATE_H

#include <memory>

namespace communication
{
	/// forward declaration of class BeaconSendingContext
	class BeaconSendingContext;

	///
	/// Abstract class for all beacon sending states
	///
	class AbstractBeaconSendingState
	{
	public:
		/// This is pseudo runtime type information used in the tests
		enum class StateType
		{
			BEACON_SENDING_INIT_STATE,
			BEACON_SENDING_TIME_SYNC_STATE,
			BEACON_SENDING_CAPTURE_ON_STATE,
			BEACON_SENDING_CAPTURE_OFF_STATE,
			BEACON_SENDING_FLUSH_SESSIONS_STATE,
			BEACON_SENDING_TERMINAL_STATE,
			BEACON_SENDING_COUNT
		};

		///
		/// Constructor
		///
		explicit AbstractBeaconSendingState(StateType type);

		///
		/// Destructor
		///
		virtual	~AbstractBeaconSendingState();


		///
		/// Execute the current state.
		///
		/// In case shutdown was requested, a state transition is performed by this method to the @ref AbstractBeaconSendingState returned by @ref AbstractBeaconSendingState::getShutdownState().
		/// @param[in] context the @ref BeaconSendingContext that takes care of state transitions
		///
		virtual void execute(BeaconSendingContext& context);

		///
		/// Get an instance of the {@ref AbstractBeaconSendingState} to which a transition is made upon shutdown request.
		/// @returns the follow-up state taking care of the shutdown
		///
		virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState() = 0;

		///
		/// Returns @c true if this state is a terminal state, @c false otherwise.
		/// @returns @c true if this state is a terminal state, @c false otherwise.
		///
		virtual bool isTerminalState() const;

		///
		/// Returns the enum for the state
		/// @returns the enum for the state
		///
		StateType getStateType() const;

	protected:
		///
		/// Execute the state - real state execution - has to overriden by subclas
		/// @param context the @see BeaconSendingContext that takes care of state transitions
		///
		virtual void doExecute(BeaconSendingContext& context) = 0;

	private:
		StateType mStateType;
	};
}
#endif