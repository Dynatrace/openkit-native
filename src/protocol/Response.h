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

#ifndef _PROTOCOL_RESPONSE_H
#define _PROTOCOL_RESPONSE_H

#include <cstdint>

namespace protocol
{
	///
	/// Abstract base class for a response to one of the 3 request types(status check, beacon send, time sync).
	///
	class Response
	{
	public:
		///
		/// Construct a response given a response code
		/// @param[in] responseCode a numerical code for the status of a request
		///
		Response(uint32_t responseCode);

		///
		/// Destructor
		///
		virtual ~Response() {};

		///
		/// Return the response code
		/// @returns the response code
		///
		virtual uint32_t getResponseCode() const;
	private:
		/// numerical response code
		uint32_t mResponseCode;
	};
}
#endif