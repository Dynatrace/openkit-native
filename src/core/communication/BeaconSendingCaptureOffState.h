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

#ifndef _CORE_COMMUNICATION_BEACONSENDINGCAPTUREOFFSTATE_H
#define _CORE_COMMUNICATION_BEACONSENDINGCAPTUREOFFSTATE_H

#include "AbstractBeaconSendingState.h"
#include "protocol/StatusResponse.h"

#include <memory>
#include <vector>
#include <chrono>

namespace core
{
	namespace communication
	{
		///
		/// State where no data is captured. Periodically issues a status request to check if capturing shall be re-enabled.
		/// The check interval is defined in @ref INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.
		///
		/// Transition to:
		///   - @ref BeaconSendingCaptureOnState if capturing is re-enabled
		///   - @ref BeaconSendingFlushSessionsState on shutdown
		///
		class BeaconSendingCaptureOffState : public AbstractBeaconSendingState
		{
		public:
			///
			/// Create CaptureOff state with default sleep behavior.
			///
			BeaconSendingCaptureOffState();

			///
			/// Create CaptureOff state with explicitly set sleep time.
			/// @param sleepTimeInMilliseconds The number of milliseconds to sleep.
			///
			BeaconSendingCaptureOffState(int64_t sleepTimeInMilliseconds);

			///
			/// Destructor
			///
			virtual ~BeaconSendingCaptureOffState() {}

			virtual void doExecute(BeaconSendingContext& context) override;

			virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState() override;

			virtual const char* getStateName() const override;

			/// The initial delay which is later on doubled between one unsuccessful attempt and the next retry
			static const std::chrono::milliseconds INITIAL_RETRY_SLEEP_TIME_MILLISECONDS;

			///
			/// Gets the sleep time set in the constructor.
			/// @remarks This returns -1, if the default sleep behavior (default constructor) is used.
			/// @return The sleep time in milliseconds.
			///
			int64_t getSleepTimeInMilliseconds() const;

		private:
			/// Handle the status response received from the server and transistion the states accordingly
			static void handleStatusResponse(BeaconSendingContext& context, std::shared_ptr<protocol::StatusResponse> statusResponse);

			/// Sleep time in milliseconds
			int64_t mSleepTimeInMilliseconds;
		};
	}
}
#endif