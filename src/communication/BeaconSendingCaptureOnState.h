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

#ifndef _COMMUNICATION_BEACONSENDINGCAPTUREONSTATE_H
#define _COMMUNICATION_BEACONSENDINGCAPTUREONSTATE_H

#include "communication/AbstractBeaconSendingState.h"

#include <vector>
#include <chrono>

namespace communication
{
	///
	/// The sending state, when init is completed and capturing is turned on.
	///
	/// Transition to:
	///   - @ref BeaconSendingTimeSyncState if BeaconSendingTimeSyncState::isTimeSyncRequired(BeaconSendingContext) is @c true
	///   - @ref BeaconSendingCaptureOffState if capturing is turned off
	///   - @ref BeaconSendingFlushSessionsState on shutdown
	///
	class BeaconSendingCaptureOnState : public AbstractBeaconSendingState
	{
	public:
		///
		/// Constructor
		///
		BeaconSendingCaptureOnState();

		///
		/// Destructor
		///
		virtual ~BeaconSendingCaptureOnState() {}

		virtual void doExecute(BeaconSendingContext& context) override;

		virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState() override;

	private:

	};
}
#endif