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

#ifndef _PROTOCOL_HTTPCLIENT_H
#define _PROTOCOL_HTTPCLIENT_H

#include "OpenKit/ILogger.h"
#include "OpenKit/ISSLTrustManager.h"
#include "core/configuration/IHTTPClientConfiguration.h"
#include "core/util/IInterruptibleThreadSuspender.h"
#include "protocol/IHTTPClient.h"

#include "curl/curl.h"

#include <vector>
#include <memory>

namespace protocol
{
	///
	/// HTTP client which abstracts the 2 basic request types:
	/// - status checkd
	/// - beacon send
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
			NEW_SESSION ///< new session request
		};

		///
		/// HTTP methods
		///
		enum class HttpMethod
		{
			GET,
			POST
		};

		///
		/// Default constructor
		/// @param[in] logger to write traces to
		/// @param[in] configuration configuration parameters for the HTTPClient
		///
		HTTPClient(
			std::shared_ptr<openkit::ILogger> logger,
			std::shared_ptr<core::configuration::IHTTPClientConfiguration> configuration,
			std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender
		);

		///
		/// Destructor
		///
		~HTTPClient() override = default;

		///
		/// Delete the copy constructor
		///
		HTTPClient(const HTTPClient&) = delete;

		///
		/// Delete the assignment operator
		///
		HTTPClient& operator = (const HTTPClient &) = delete;

		std::shared_ptr<IStatusResponse> sendStatusRequest(const protocol::IAdditionalQueryParameters& additionalParameters) override;

		std::shared_ptr<IStatusResponse> sendBeaconRequest(
			const core::UTF8String& clientIPAddress,
			const core::UTF8String& beaconData,
			const protocol::IAdditionalQueryParameters& additionalParameters
		) override;

		std::shared_ptr<IStatusResponse> sendNewSessionRequest(const protocol::IAdditionalQueryParameters& additionalParameters) override;

		///
		/// Perform global initialization.
		/// @remarks This method expects to be called before any other operation.
		///
		static void globalInit();

		///
		/// Perform global destruction.
		/// @remarks This method expects to be called after the last operation on the @c HTTPClient.
		///
		static void globalDestroy();

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
		std::shared_ptr<IStatusResponse> sendRequestInternal(RequestType requestType, const core::UTF8String& url, const core::UTF8String& clientIPAddress, const core::UTF8String& beaconData, const HttpMethod method);

		///
		/// Build URL used for status check and beacon send requests
		/// @param[in,out] monitorURL the url to build
		/// @param[in] baseURL the beacon base url without the query string
		/// @param[in] applicationID
		/// @param[in] serverID
		///
		static void buildMonitorURL(core::UTF8String& monitorURL, const core::UTF8String& baseURL, const core::UTF8String& applicationID, uint32_t serverID);

		static void buildNewSessionURL(core::UTF8String& newSessionURL, const core::UTF8String& baseURL, const core::UTF8String& applicationID, uint32_t serverID);

		static core::UTF8String appendAdditionalQueryParameters(const core::UTF8String& baseUrl, const protocol::IAdditionalQueryParameters& parameters);

		static void appendQueryParam(core::UTF8String& url, const char* key, const core::UTF8String& value);

		std::shared_ptr<IStatusResponse> handleResponse(RequestType requestType, int32_t httpCode, const std::string& buffer, const IStatusResponse::ResponseHeaders& responseHeaders);

		static size_t readFunction(void *ptr, size_t elementSize, size_t numberOfElements, void* userPtr);

		std::shared_ptr<IStatusResponse> unknownErrorResponse(RequestType requestType);

	private:

		/// Logger to write traces to
		std::shared_ptr<openkit::ILogger> mLogger;

		/// interruptable thread suspender
		std::shared_ptr<core::util::IInterruptibleThreadSuspender> mThreadSuspender;

		/// easy handle to the CURL session
		CURL * mCurl;

		/// the server ID
		const uint32_t mServerID;

		/// URL used for status check and beacon send requests
		core::UTF8String mMonitorURL;

		/// buffer used for curl's read function
		std::vector<unsigned char> mReadBuffer;

		/// read position in the read buffer
		size_t mReadBufferPos;

		/// how the peer's TSL/SSL certificate and the hostname shall be trusted
		std::shared_ptr<openkit::ISSLTrustManager> mSSLTrustManager;

		/// URL for new session requests
		core::UTF8String mNewSessionURL;
	};

}

#endif