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

#ifndef _CORE_COMMUNICATION_BEACONSENDINGCAPTUREONSTATE_H
#define _CORE_COMMUNICATION_BEACONSENDINGCAPTUREONSTATE_H

#include "AbstractBeaconSendingState.h"
#include "IBeaconSendingContext.h"
#include "protocol/IStatusResponse.h"

#include <memory>
#include <vector>
#include <chrono>

namespace core
{
	namespace communication
	{
		///
		/// The sending state, when init is completed and capturing is turned on.
		///
		/// Transition to:
		///   - @ref BeaconSendingCaptureOffState if capturing is turned off
		///   - @ref BeaconSendingFlushSessionsState on shutdown
		///
		class BeaconSendingCaptureOnState
			: public AbstractBeaconSendingState
		{
		public:
			///
			/// Constructor
			///
			BeaconSendingCaptureOnState();

			///
			/// Destructor
			///
			~BeaconSendingCaptureOnState() override = default;

			void doExecute(IBeaconSendingContext& context) override;

			std::shared_ptr<IBeaconSendingState> getShutdownState() override;

			const char* getStateName() const override;

		private:
			///
			/// Send all sessions which have been finished previously.
			/// @param[in] context the state context
			///
			std::shared_ptr<protocol::IStatusResponse> sendFinishedSessions(IBeaconSendingContext& context);

			///
			/// Check if the send interval (configured by server) has expired and start to send open sessions if it has expired.
			/// @param[in] context the state context
			///
			std::shared_ptr<protocol::IStatusResponse> sendOpenSessions(IBeaconSendingContext& context);

			///
			/// Handle the status response received from the server and transition the states accordingly
			/// @param[in] beacon sending context
			/// @param[in] response to process
			///
			static void handleStatusResponse(
				IBeaconSendingContext& context,
				std::shared_ptr<protocol::IStatusResponse> statusResponse
			);

			///
			/// Check if new sessions are allowed to report data
			/// @param[in] context beacon sending context
			///
			std::shared_ptr<protocol::IStatusResponse> sendNewSessionRequests(
				IBeaconSendingContext& context
			);
		};
	}
}
#endif