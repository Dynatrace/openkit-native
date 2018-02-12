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

#ifndef _COMMUNICATION_BEACONSENDINGTERMINALSTATE_H
#define _COMMUNICATION_BEACONSENDINGTERMINALSTATE_H

#include "communication/AbstractBeaconSendingState.h"

namespace communication {
	///
	/// Terminal state once beacon sending is finished and shutdown is requested.
	/// NOTE: The initial state is used to retrieve the configuration from the server and update the configuration.
	///
	class BeaconSendingTerminalState : public AbstractBeaconSendingState
	{
	public:
		///
		/// Constructor
		///
		BeaconSendingTerminalState();

		///
		/// Destructor
		///
		virtual ~BeaconSendingTerminalState();

		///
		/// Execute this state
		/// @param context Instance of the BeaconSendingContext responsible for state transitions
		///
		virtual void doExecute(BeaconSendingContext& context) override;

		///
		/// Return the shutdown state
		/// @returns an instance of @c AbstractBeaconSendingState handling the shutdown after the init state
		///
		virtual std::unique_ptr<AbstractBeaconSendingState> getShutdownState() override;

		///
		/// Return a flag if this is a shutdown state.
		/// @return @s true if this state is a shutdown state, @s false if not
		///
		virtual bool isAShutdownState() override;
	};
}
#endif