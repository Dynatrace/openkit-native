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

#ifndef _CORE_COMMUNICATION_BEACONSENDINGFLUSHSESSIONSTATE_H
#define _CORE_COMMUNICATION_BEACONSENDINGFLUSHSESSIONSTATE_H

#include "AbstractBeaconSendingState.h"

#include <vector>
#include <chrono>

namespace core
{
	namespace communication
	{
		///
		/// In this state open sessions are finished. After that all sessions are sent to the server.
		///
		/// Transition to:
		///   - @ref BeaconSendingTerminalState
		///
		class BeaconSendingFlushSessionsState : public AbstractBeaconSendingState
		{
		public:
			///
			/// Constructor
			///
			BeaconSendingFlushSessionsState();

			///
			/// Destructor
			///

			virtual ~BeaconSendingFlushSessionsState() {}

			virtual void doExecute(BeaconSendingContext& context) override;

			virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState() override;

			virtual const char* getStateName() const override;

		private:

		};
	}
}
#endif