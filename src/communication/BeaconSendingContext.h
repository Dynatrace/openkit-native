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
		/// @param[in] initialState the initial state to use in this sending context
		///
		BeaconSendingContext(std::unique_ptr<AbstractBeaconSendingState> initialState);

		///
		/// Register a state following the current state once the current state finished
		/// @param nextState instance of the @s AbstractBeaconSendingState that follows after the current state
		///
		void setNextState(std::unique_ptr<AbstractBeaconSendingState> nextState);

		///
		/// Return a flag if the current state of this context is a terminal state
		/// @returns @s true if the current state is a terminal state
		///
		bool isInTerminalState();

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
		bool isShutdownRequested();

	private:
		/// instance of @s AbstractBeaconSendingState with the current state
		std::unique_ptr<AbstractBeaconSendingState> mCurrentState;

		/// Flag if the current state is a terminal state
		bool mIsInTerminalState;

		/// Atomic shutdown flag
		std::atomic<bool> mShutdown;
	};
}
#endif