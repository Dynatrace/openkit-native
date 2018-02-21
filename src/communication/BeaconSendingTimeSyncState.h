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

#ifndef _COMMUNICATION_BEACONSENDINGTIMESYNCSTATE_H
#define _COMMUNICATION_BEACONSENDINGTIMESYNCSTATE_H

#include "communication/AbstractBeaconSendingState.h"

namespace communication {

	///
	/// The state responsible for the time sync. In this state a time sync is performed.
	/// If capturing is enabled this state transitions to BeaconSendingCaptureOnState
	/// If capturing is disabled this tate transitions to BeaconSendingCaptureOffState
	/// If this is not the initial time sync and shutdown was requested this state transitions to BeaconSendingFlushSessionsState
	/// If this is the initial time sync and shutdown was requested this state transitions to BeaconSendingTerminalState
	///
	class BeaconSendingTimeSyncState : public AbstractBeaconSendingState
	{
	public:
		///
		/// Constructor
		///
		BeaconSendingTimeSyncState();

		///
		/// Constructor with a flag if this is the initial time sync
		/// @param[in] initialSync @c true if 
		///
		BeaconSendingTimeSyncState(bool initialSync);

		///
		/// Destructor
		///
		virtual ~BeaconSendingTimeSyncState() {};

		///
		/// Execute this state
		/// @param context Instance of the BeaconSendingContext responsible for state transitions
		///
		virtual void doExecute(BeaconSendingContext& context) override;

		///
		/// Return the shutdown state
		/// @returns an instance of @c AbstractBeaconSendingState handling the shutdown after the init state
		///
		virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState() override;

		///
		/// Return a flag if this is a shutdown state.
		/// @return @c true if this state is a shutdown state, @c false if not
		///
		virtual bool isAShutdownState() override;

	private:
		///
		/// Flag if this is the first time time sync is performed
		///
		bool mInitialTimeSync;
	};

}
#endif