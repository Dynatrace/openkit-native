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

#ifndef _PROTOCOL_STATUSRESPONSE_H
#define _PROTOCOL_STATUSRESPONSE_H

#include "Response.h"
#include "core/UTF8String.h"

namespace protocol {
	///
	/// Implements a status response which is sent for the request types status check and beacon send.
	/// A status response encapsulates settings contained in the status response
	///
	class StatusResponse : public Response
	{
	public:

		///
		/// Default constructor
		///
		StatusResponse();

		///
		/// Construct a status response using a response code and string
		/// @param[in] response the response string obtained from the server
		/// @param[in] responseCode a numerical response code
		///
		StatusResponse(const core::UTF8String response, uint32_t responseCode);

	public:
		///
		/// Get a flag if capturing is enabled by the cluster
		/// @returns @c true if capturing is enabled, @c false is capturing is not enabled
		///
		bool isCapture() const;

		///
		/// Get the send interval
		/// @returns send interval in seconds
		/// 
		int32_t getSendInterval() const;

		///
		/// Get the monitor name
		/// @returns the monitor name
		///
		const core::UTF8String& getMonitorName() const;

		///
		/// Get the server id
		/// @returns the server id
		///
		int32_t getServerID() const;

		///
		/// Get the maximum beacon size
		/// @returns the maximum beacon size in kilobytes
		///
		int32_t getMaxBeaconSize() const;

		///
		/// Get a flag if errors should be reported
		/// @returns @c true if errors are reported to the cluster, @c false if errors are not reported
		///
		bool isCaptureErrors() const;

		///
		/// Get a flag if crashes should be reported
		/// @returns @c true if errors are reported to the cluster, @c false if errors are not reported
		///
		bool isCaptureCrashes() const;

	private:
		///
		/// Parse the response string for further details
		/// @param[in] response the response string obtained from the server
		///
		void parseResponse(const core::UTF8String response);
	private:

		/// capture on/off
		bool mCapture;

		/// send interval
		int32_t mSendInterval;

		/// monitor name
		core::UTF8String mMonitorName;

		/// server id
		int32_t mServerID;
		
		/// maximum beacon size
		int32_t mMaxBeaconSize;

		/// capture errors
		bool mCaptureErrors;

		/// capture crashes
		bool mCaptureCrashes;
	};
}

#endif