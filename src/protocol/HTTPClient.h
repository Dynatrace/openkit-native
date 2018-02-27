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

#ifndef _PROTOCOL_HTTPCLIENT_H
#define _PROTOCOL_HTTPCLIENT_H

#include <vector>
#include <string.h>

#include "protocol/IHTTPClient.h"
#include "protocol/ssl/ISSLTrustManager.h"
#include "curl/curl.h"

namespace protocol {
	///
	/// HTTP client which abstracts the 3 basic request types:
	/// - status check
	/// - beacon send
	/// - time sync
	///
	class HTTPClient : public IHTTPClient
	{
	public:

		///
		/// the type of request sent to the server
		///
		enum class RequestType
		{
			STATUS, ///< status check request
			BEACON, ///< beacon send request
			TIMESYNC ///< time sync request
		};

		///
		/// HTTP methods
		///
		enum HttpMethod
		{
			GET,
			POST
		};

		///
		/// Default constructor
		/// @param[in] configuration configuration parameters for the HTTPClient
		///
		HTTPClient(std::shared_ptr<configuration::HTTPClientConfiguration> configuration);

		///
		/// Destructor
		///
		virtual ~HTTPClient() ;

		///
		/// Delete the copy constructor
		///
		HTTPClient(const HTTPClient&) = delete;

		///
		/// Delete the assignment operator
		///
		HTTPClient& operator = (const HTTPClient &) = delete;

		///
		/// sends a status check request and returns a status response
		/// @returns a status response with the response data for the request or @c nullptr on error
		///
		virtual std::unique_ptr<StatusResponse> sendStatusRequest() override;

		///
		/// sends a beacon send request and returns a status response
		/// @param[in] clientIPAddress the client IP address
		/// @param[in] beaconData the beacon payload
		/// @returns a status response with the response data for the request or @c nullptr on error
		///
		virtual std::unique_ptr<StatusResponse> sendBeaconRequest(const core::UTF8String& clientIPAddress, const core::UTF8String& beaconData) override;

		///
		/// sends a timesync request and returns a timesync response
		/// @returns a timesync response with the response data for the request or @c nullptr on error
		///
		virtual std::unique_ptr<TimeSyncResponse> sendTimeSyncRequest() override;

	private:

		///
		/// sends a status check request and returns a status response
		/// @param[in] requestType the type of request sent to the server
		/// @param[in] url the url where to send the request to
		/// @param[in] clientIPAddress optional the IP address of the client. If provided, this is sent in the custom HTTP header "X-Client-IP"
		/// @param[in] beaconData optional data to send in the HTTP POST. Data will be gzip compressed.
		/// @param[in] method the HTTP method to use. Currently either POST or GET
		/// @returns a status response with the response data for the request or @c nullptr on error
		///
		std::unique_ptr<Response> sendRequestInternal(const RequestType requestType, const core::UTF8String& url, const core::UTF8String& clientIPAddress, const core::UTF8String& beaconData, const HttpMethod method);

		void buildMonitorURL(core::UTF8String& monitorURL, const core::UTF8String& baseURL, const core::UTF8String& applicationID, uint32_t serverID);

		void buildTimeSyncURL(core::UTF8String& monitorURL, const core::UTF8String& baseURL);

		void appendQueryParam(core::UTF8String& url, const char* key, const char* vaalue);

		std::unique_ptr<Response> handleResponse(uint64_t httpCode, const std::string& buffer);

		static size_t readFunction(void *ptr, size_t elementSize, size_t numberOfElements, void* userPtr);

	private:

		/// easy handle to the CURL session
		CURL * mCurl;

		/// the server ID
		const uint32_t mServerID;

		/// URL used for status check and beacon send requests
		core::UTF8String mMonitorURL;

		/// the beacon URL
		core::UTF8String mTimeSyncURL;

		/// buffer used for curl's read function
		std::vector<unsigned char> mReadBuffer;

		/// read position in the read buffer
		size_t mReadBufferPos;

		/// how the peer's TSL/SSL certificate and the hostname shall be trusted
		const std::shared_ptr<ISSLTrustManager> mSSLTrustManager;
	};

}

#endif