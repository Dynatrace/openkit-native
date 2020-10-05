/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _PROTOCOL_RESPONSEATTRIBUTE_H
#define _PROTOCOL_RESPONSEATTRIBUTE_H



namespace protocol
{
	///
	/// Specifies an attribute in the @ref protocol::IResponseAttributes sent by the server
	///
	enum class ResponseAttribute
	{
		///
		/// Maximum POST body size when sending beacon data.
		///
		MAX_BEACON_SIZE,

		///
		/// Maximum duration after which a session is split.
		///
		MAX_SESSION_DURATION,

		///
		/// Maximum number of top level actions after which a session is split.
		///
		MAX_EVENTS_PER_SESSION,

		///
		/// Idle timeout after which a session is split.
		///
		SESSION_IDLE_TIMEOUT,

		///
		/// Interval at which beacon data is sent to the server.
		///
		SEND_INTERVAL,

		///
		/// Version of the visit store to be used.
		///
		VISIT_STORE_VERSION,

		///
		/// Indicator whether capturing data is allowed or not.
		///
		IS_CAPTURE,

		///
		/// Indicator whether crashes should be captured or not.
		///
		IS_CAPTURE_CRASHES,

		///
		/// Indicator whether errors should be captured or not.
		///
		IS_CAPTURE_ERRORS,

		///
		/// Multiplicity
		///
		MULTIPLICITY,

		///
		/// The ID of the server to which data should be sent to.
		///
		SERVER_ID,

		///
		/// Timestamp of the configuration sent by the server.
		///
		TIMESTAMP
	};

	///
	/// Holds all literals of @ref ResponseAttribute
	///
	static constexpr ResponseAttribute ALL_RESPONSE_ATTRIBUTES[] =
	{
		ResponseAttribute::MAX_BEACON_SIZE,
		ResponseAttribute::MAX_SESSION_DURATION,
		ResponseAttribute::MAX_EVENTS_PER_SESSION,
		ResponseAttribute::SESSION_IDLE_TIMEOUT,
		ResponseAttribute::SEND_INTERVAL,
		ResponseAttribute::VISIT_STORE_VERSION,
		ResponseAttribute::IS_CAPTURE,
		ResponseAttribute::IS_CAPTURE_CRASHES,
		ResponseAttribute::IS_CAPTURE_ERRORS,
		ResponseAttribute::MULTIPLICITY,
		ResponseAttribute::SERVER_ID,
		ResponseAttribute::TIMESTAMP
	};
}


#endif
