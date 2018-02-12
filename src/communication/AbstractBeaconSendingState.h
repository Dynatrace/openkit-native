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

#include "BeaconSendingContext.h"

namespace communication {

	///
	/// Abstract class for all beacon sending states
	///
	class AbstractBeaconSendingState
	{
	public:

		///
		///constructor
		///
		AbstractBeaconSendingState() {}

		///
		/// destructor
		///
		virtual	~AbstractBeaconSendingState() {}


		///
		/// execute the state, exit in case of shutdown requests
		/// @param context the  BeaconSendingContext that takes care of state transitions
		///
		void execute(BeaconSendingContext& context)
		{
			doExecute(context);

			if (context.isShutdownRequested()) {
				context.setNextState(getShutdownState());
			}
		}

		///
		/// Get an instance of the shutdown state of the  AbstractBeaconSendingState that is called upon shutdown
		/// @returns the follow-up state taking care of the shutdown
		///
		virtual std::unique_ptr<AbstractBeaconSendingState> getShutdownState() = 0;

		///
		/// Return a flag whether the current state is a shutdown state or not.
		///
		virtual bool isAShutdownState() = 0;

	protected:
		///
		/// execute the state - real state execution - has to overriden by subclas
		/// @param context the  BeaconSendingContext that takes care of state transitions
		///
		virtual void doExecute(BeaconSendingContext& context) = 0;
	};
}
#endif