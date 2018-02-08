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

#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"
#include "configuration/Configuration.h"

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
		/// @param[in] httpClientProvider provider for HTTPClient objects
		/// @param[in] timingProvider utility class for timing related stuff
		/// @param[in] configuration general configuration options
		///
		BeaconSendingContext(providers::IHTTPClientProvider& httpClientProvider,
			providers::ITimingProvider& timingProvider,
			configuration::Configuration configuration);

		///
		/// Register a state following the current state once the current state finished
		/// @param nextState instance of the @s AbstractBeaconSendingState that follows after the current state
		///
		void setNextState(std::unique_ptr<AbstractBeaconSendingState> nextState);

		///
		/// Return a flag if the current state of this context is a terminal state
		/// @returns @s true if the current state is a terminal state
		///
		bool isInTerminalState() const;

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
		bool isShutdownRequested() const;

		///
		/// Return the currently used @s Configuration
		/// @return configuration isntance
		///
		const configuration::Configuration& getConfiguration() const;

		///
		/// Returns the HTTPClient created by the current BeaconSendingContext
		/// @returns a shared pointer to the HTTTP client created by the BeaconSendingContext
		///
		std::unique_ptr<protocol::HTTPClient> getHTTPClient();

	private:
		/// instance of @s AbstractBeaconSendingState with the current state
		std::unique_ptr<AbstractBeaconSendingState> mCurrentState;

		/// Flag if the current state is a terminal state
		bool mIsInTerminalState;

		/// Atomic shutdown flag
		std::atomic<bool> mShutdown;

		/// The configuration to use
		configuration::Configuration mConfiguration;

		/// IHTTPClientProvider responsible for creating instances of HTTPClient
		providers::IHTTPClientProvider& mHTTPClientProvider;

		/// TimingPRovider used by the BeaconSendingContext
		providers::ITimingProvider& mTimingProvider;
	};
}
#endif