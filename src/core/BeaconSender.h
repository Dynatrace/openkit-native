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

#ifndef CORE_BEACONSENDER_H
#define CORE_BEACONSENDER_H

#include <thread>

#include "communication/BeaconSendingContext.h"
#include "configuration/Configuration.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"

namespace core {
	///
	/// The BeaconSender runs a thread executing the beacon sending states
	///
	class BeaconSender
	{
	public:
		///
		/// Default constructor
		/// @param[in] configuration general configuration options
		/// @param[in] httpClientProvider the provider for HTTPClient instances
		/// @param[in] timingProvider utility requried for timing related stuff
		///
		BeaconSender(configuration::Configuration& configuration,
			providers::IHTTPClientProvider& httpClientProvider,
			providers::ITimingProvider& timingProvider);

		///
		/// Destructor
		///
		~BeaconSender();

		///
		/// Initialize this BeaconSender
		///
		bool initialize();

		///
		/// Shutdown this instance of the BeaconSender
		///
		void shutdown();

	private:

		/// Beacon sending context managing the state transitions and shutdown
		std::shared_ptr<communication::BeaconSendingContext> mBeaconSendingContext;

		/// thread instance running the beacon sending state machine
		std::unique_ptr<std::thread> mSendingThread;
	};
}
#endif