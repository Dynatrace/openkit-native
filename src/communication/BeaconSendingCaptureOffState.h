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

#ifndef _COMMUNICATION_BEACONSENDINGCAPTUREOFFSTATE_H
#define _COMMUNICATION_BEACONSENDINGCAPTUREOFFSTATE_H

#include "communication/AbstractBeaconSendingState.h"

#include <vector>
#include <chrono>

namespace communication
{
	///
	/// State where no data is captured. Periodically issues a status request to check if capturing shall be re-enabled.
	/// The check interval is defined in @ref INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.
	///
	/// Transition to:
	///   - @ref BeaconSendingCaptureOnState if capturing is re-enabled
	///   - @ref BeaconSendingFlushSessionsState on shutdown
	///   - @ref BeaconSendingTimeSyncState if initial time sync failed
	///
	class BeaconSendingCaptureOffState : public AbstractBeaconSendingState
	{
	public:
		///
		/// Constructor
		///
		BeaconSendingCaptureOffState();

		///
		/// Destructor
		///
		virtual ~BeaconSendingCaptureOffState() {}

		virtual void doExecute(BeaconSendingContext& context) override;

		virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState() override;

		/// The initial delay which is later on doubled between one unsuccessful attempt and the next retry
		static const std::chrono::milliseconds INITIAL_RETRY_SLEEP_TIME_MILLISECONDS;
		
	private:

	};
}
#endif