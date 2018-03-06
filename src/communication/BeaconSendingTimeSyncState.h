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

#include <chrono>
#include <vector>

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

		///
		/// Uses the BeaconSendingContext to determine if a time sync is required
		/// @param[in] context BeaconSendingContext used for the check
		/// @returns @c true if time sync is required, @c false if time sync is not required
		///
		bool isTimeSyncRequired(BeaconSendingContext& context);

	private:

		///
		/// Make a transition to the next state based on the capture enable flag of the BeaconSendingContext
		/// @param[in] context the BeaconSendingContext instance to perform the state transition on
		///
		void setNextState(BeaconSendingContext& context);

		///
		/// Handle the received timesync responses
		/// @param[in] context the BeaconSendingContext to apply the changes on
		/// @param[in] timeSyncOffsets the received offsets
		///
		void handleTimeSyncResponses(BeaconSendingContext& context, std::vector<int64_t>& timeSyncOffsets);

		///
		/// Calculates the cluster time offset from the list of time sync offsets 
		/// @param[in] timeSyncOffsets list of the retrieved offsets
		/// @returns the cluster time offset
		///
		int64_t computeClusterTimeOffset(std::vector<int64_t>& timeSyncOffsets);

		///
		/// In case of a erroneous time sync request 
		/// @paramp[in] context BeaconSendingContext keeping the state information
		///
		void handleErroneousTimeSyncRequest(BeaconSendingContext& context);

		///
		/// Execute the time synchronisation requests (HTTP requests).
		/// @param[in] context the BeconSendingContext used
		/// @returns a vector of integers with the time sync offsets
		///
		std::vector<int64_t> executeTimeSyncRequests(BeaconSendingContext& context);

	public:
		///
		/// The tine sync interval in milliseconds
		///
		static std::chrono::milliseconds TIME_SYNC_INTERVAL_IN_MILLIS;

		///
		/// number of time syncs request for a successful time sync
		///
		static uint32_t REQUIRED_TIME_SYNC_REQUESTS;

		///
		/// initial retry sleep time
		///
		static std::chrono::milliseconds INITIAL_RETRY_SLEEP_TIME_MILLISECONDS;

	private:
		///
		/// Flag if this is the first time time sync is performed
		///
		bool mInitialTimeSync;


	};

}
#endif