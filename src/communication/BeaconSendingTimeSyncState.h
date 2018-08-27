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
#include "protocol/TimeSyncResponse.h"

#include <chrono>
#include <vector>
#include <memory>

namespace communication {

	///
	/// The state responsible for the time sync. In this state a time sync is performed.
	///
	/// Transition to:
	///   - @ref BeaconSendingCaptureOnState if capturing is enabled (@ref BeaconSendingContext::isCaptureOn() == @c true)
	///   - @ref BeaconSendingCaptureOffState if capturing is disabled (@ref BeaconSendingContext::isCaptureOn() == @c false) or time sync failed
	///   - @ref BeaconSendingFlushSessionsState on shutdown if not initial time sync
	///   - @ref BeaconSendingTerminalState on shutdown if initial time sync
	///
	class BeaconSendingTimeSyncState : public AbstractBeaconSendingState
	{
	public:
		///
		/// Default Constructor
		///
		BeaconSendingTimeSyncState();

		///
		/// Constructor with a flag if this is the initial time sync
		/// @param[in] initialSync @c true if initial time sync
		///
		BeaconSendingTimeSyncState(bool initialSync);

		///
		/// Destructor
		///
		virtual ~BeaconSendingTimeSyncState() {};

		virtual void doExecute(BeaconSendingContext& context) override;

		virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState() override;

		virtual const char* getStateName() const override;

		///
		/// Uses the @ref BeaconSendingContext to determine if a time sync is required
		/// @param[in] context @ref BeaconSendingContext used for the check
		/// @returns @c true if time sync is required, @c false if time sync is not required
		///
		static bool isTimeSyncRequired(BeaconSendingContext& context);

	private:

		///
		/// Container class storing data for processing requests.
		///
		class TimeSyncRequestsResponse
		{
		public:

			///
			/// Default constructor.
			///
			TimeSyncRequestsResponse();


			///
			/// List storing time sync offsets.
			///
			std::vector<int64_t> mTimeSyncOffsets;

			///
			/// TimeSync response which is only stored in case of "too many requests" response, otherwise it's always @c nullptr.
			///
			std::shared_ptr<protocol::TimeSyncResponse> mResponse;
		};

		///
		/// Make a transition to the next state based on the capture enable flag of the @ref BeaconSendingContext
		/// @param[in] context the BeaconSendingContext instance to perform the state transition on
		///
		void setNextState(BeaconSendingContext& context);

		///
		/// Handle the received timesync responses
		/// @param[in] context the @ref BeaconSendingContext to apply the changes on
		/// @param[in] response the received offsets or error response.
		///
		void handleTimeSyncResponses(BeaconSendingContext& context, TimeSyncRequestsResponse& response);

		///
		/// Calculates the cluster time offset from the list of time sync offsets 
		/// @param[in] timeSyncOffsets list of the retrieved offsets
		/// @returns the cluster time offset
		///
		int64_t computeClusterTimeOffset(std::vector<int64_t>& timeSyncOffsets);

		///
		/// In case of a erroneous time sync request 
		/// @param[in] context @ref BeaconSendingContext keeping the state information
		///
		void handleErroneousTimeSyncRequest(std::shared_ptr<protocol::TimeSyncResponse> response, BeaconSendingContext& context);

		///
		/// Execute the time synchronisation requests (HTTP requests).
		/// @param[in] context the @ref BeaconSendingContext used
		/// @returns a vector of integers with the time sync offsets
		///
		TimeSyncRequestsResponse executeTimeSyncRequests(BeaconSendingContext& context);

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
