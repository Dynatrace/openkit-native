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

#include "HTTPClient.h"
#include "HTTPResponseParser.h"
#include "ProtocolConstants.h"
#include "core/util/Compressor.h"
#include "core/util/URLEncoding.h"
#include "core/util/StringUtil.h"
#include "protocol/IStatusResponse.h"
#include "protocol/ResponseParser.h"
#include "protocol/StatusResponse.h"
#include "protocol/ssl/SSLStrictTrustManager.h"

#include <cstdint>
#include <chrono>
#include <thread>
#include <algorithm>
#include <string>
#include <cctype>
#include <limits>
#include <string.h>

// connection constants
constexpr uint32_t MAX_SEND_RETRIES = 3; // max number of retries of the HTTP GET or POST operation
constexpr uint32_t RETRY_SLEEP_TIME = 200; // retry sleep time in ms
constexpr uint64_t CONNECT_TIMEOUT = 5; // Time-out connect operations after this amount of seconds
constexpr uint64_t READ_TIMEOUT = 30; // Time-out the read operation after this amount of seconds

using namespace protocol;
using namespace base::util;

HTTPClient::HTTPClient
(
	std::shared_ptr<openkit::ILogger> logger,
	const std::shared_ptr<core::configuration::IHTTPClientConfiguration> configuration
)
	: mLogger(logger)
	, mCurl(nullptr)
	, mServerID(configuration->getServerID())
	, mMonitorURL()
	, mReadBuffer()
	, mReadBufferPos(0)
	, mSSLTrustManager(nullptr)
	, mNewSessionURL()
{
	// build the beacon URLs
	buildMonitorURL(mMonitorURL, configuration->getBaseURL(), configuration->getApplicationID(), mServerID);
	buildNewSessionURL(mNewSessionURL, configuration->getBaseURL(), configuration->getApplicationID(), mServerID);

	// if configuration provides a trust manager use it, else use strict trust manager
	auto trustManagerFromConfiguration = configuration->getSSLTrustManager();
	if (trustManagerFromConfiguration != nullptr)
	{
		mSSLTrustManager = trustManagerFromConfiguration;
	}
	else
	{
		mSSLTrustManager = std::make_shared<protocol::SSLStrictTrustManager>();
	}
}

std::shared_ptr<IStatusResponse> HTTPClient::sendStatusRequest(const protocol::IAdditionalQueryParameters& additionalParameters)
{
	auto url = appendAdditionalQueryParameters(mMonitorURL, additionalParameters);
	auto response = sendRequestInternal(RequestType::STATUS, url, core::UTF8String(), core::UTF8String(), HttpMethod::GET);
	if (response == nullptr)
	{
		response = StatusResponse::createErrorResponse(mLogger, std::numeric_limits<int32_t>::max());
	}

	return response;
}

std::shared_ptr<IStatusResponse> HTTPClient::sendBeaconRequest(
	const core::UTF8String& clientIPAddress,
	const core::UTF8String& beaconData,
	const protocol::IAdditionalQueryParameters& additionalParameters)
{
	auto url = appendAdditionalQueryParameters(mMonitorURL, additionalParameters);
	auto response = sendRequestInternal(RequestType::BEACON, url, clientIPAddress, beaconData, HttpMethod::POST);
	if (response == nullptr)
	{
		response = StatusResponse::createErrorResponse(mLogger, std::numeric_limits<int32_t>::max());
	}

	return response;
}

std::shared_ptr<IStatusResponse> HTTPClient::sendNewSessionRequest(const protocol::IAdditionalQueryParameters& additionalParameters)
{
	auto url = appendAdditionalQueryParameters(mNewSessionURL, additionalParameters);
	auto response = sendRequestInternal(RequestType::NEW_SESSION, url, core::UTF8String(), core::UTF8String(), HttpMethod::GET);
	if (response == nullptr)
	{
		response = StatusResponse::createErrorResponse(mLogger, std::numeric_limits<int32_t>::max());
	}

	return response;
}

void HTTPClient::globalInit()
{
	// set up the program environment that libcurl needs. In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);
}

void HTTPClient::globalDestroy()
{
	curl_global_cleanup();
}

///
/// Callback function for reading data to upload (=the data in a POST request).
/// @param[in,out] ptr where the data to POST is written
/// @param[in] elementSize of the data
/// @param[in] numberOfElements number of data (size of the written data = elementSize * numberOfElements)
/// @param[in] userPtr the user data to upload is read from there
/// @return the size of the delievered data
///
size_t HTTPClient::readFunction(void *ptr, size_t elementSize, size_t numberOfElements, void* userPtr)
{
	if (userPtr)
	{
		auto _this = (HTTPClient*)userPtr;
		size_t available = (_this->mReadBuffer.size() - _this->mReadBufferPos);

		if (available > 0)
		{
			size_t written = std::min(elementSize * numberOfElements, available);
			memcpy(ptr, ((char*)(_this->mReadBuffer.data())) + _this->mReadBufferPos, written);
			_this->mReadBufferPos += written;
			return written;
		}
	}

	return 0;
}

///
/// Local callback function for writing received data (=the response).
/// @param[in] ptr to the delivered data
/// @param[in] elementSize of the data
/// @param[in] numberOfElements number of data (size of the delivered data = size * nmemb)
/// @param[in,out] data to write the received data to
/// @return the size of the delievered data
///
static size_t writeFunction(char *ptr, size_t elementSize, size_t numberOfElements, void *userdata)
{
	if (userdata != nullptr)
	{
		return reinterpret_cast<HTTPResponseParser*>(userdata)->responseBodyData(ptr, elementSize, numberOfElements);
	}

	return 0;
}

///
/// Local callback function invoked whenever a HTTP response header line was received.
/// @param[in] buffer to the delivered data
/// @param[in] elementSize of the data
/// @param[in] numberOfElements number of data (size of the delivered data = size * nmemb)
/// @param[in,out] data to write the received data to
/// @return the size of the delievered data
static size_t headerFunction(char *buffer, size_t elementSize, size_t numberOfElements, void *userdata)
{
	if (userdata != nullptr)
	{
		return reinterpret_cast<HTTPResponseParser*>(userdata)->responseHeaderData(buffer, elementSize, numberOfElements);
	}

	return elementSize * numberOfElements;
}

//TODO: stefan.eberl - use the request type or rethink design
std::shared_ptr<IStatusResponse> HTTPClient::sendRequestInternal(HTTPClient::RequestType requestType, const core::UTF8String& url, const core::UTF8String& clientIPAddress, const core::UTF8String& beaconData, const HTTPClient::HttpMethod method)
{
	if (mLogger->isDebugEnabled())
	{
		switch(requestType)
		{
		case HTTPClient::RequestType::STATUS:
			mLogger->debug("HTTPClient sendRequestInternal() - HTTP status request: %s", url.getStringData().c_str());
			break;
		case HTTPClient::RequestType::BEACON:
			mLogger->debug("HTTPClient sendRequestInternal() - HTTP beacon request: %s", url.getStringData().c_str());
			break;
		case HTTPClient::RequestType::NEW_SESSION:
			mLogger->debug("HTTPClient sendRequestInternal() - HTTP new session request: %s", url.getStringData().c_str());
			break;
		}
	}

	// init the curl session - get the curl handle
	mCurl = curl_easy_init();

	if (!mCurl)
	{
		// Abort and cleanup if CURL cannot be initialized
		mLogger->error("HTTPClient sendRequestInternal() - curl_easy_init() failed");
		return HTTPClient::unknownErrorResponse(requestType);
	}

	long httpCode = 0L;
	uint32_t retryCount = 0;
	do
	{
		// Set the connection parameters (URL, timeouts, etc.)
		curl_easy_setopt(mCurl, CURLOPT_URL, url.getStringData().c_str());
		curl_easy_setopt(mCurl, CURLOPT_CONNECTTIMEOUT, CONNECT_TIMEOUT);
		curl_easy_setopt(mCurl, CURLOPT_TIMEOUT, READ_TIMEOUT);
		// allow servers to send compressed data
		curl_easy_setopt(mCurl, CURLOPT_ACCEPT_ENCODING, "");
		// SSL/TSL certificate handling
		mSSLTrustManager->applyTrustManager(mCurl);

		HTTPResponseParser responseParser;
		// To retrieve the response headers
		curl_easy_setopt(mCurl, CURLOPT_HEADERFUNCTION, headerFunction);
		curl_easy_setopt(mCurl, CURLOPT_HEADERDATA, &responseParser);
		// To retrieve the response
		curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, writeFunction);
		curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &responseParser);


		// Set the custom HTTP header with the client IP address, if provided
		struct curl_slist *list = nullptr;
		if (!clientIPAddress.empty())
		{
			core::UTF8String xClientId("X-Client-IP: ");
			xClientId.concatenate(clientIPAddress);
			list = curl_slist_append(list, xClientId.getStringData().c_str());
		}

		if (method == HttpMethod::POST)
		{
			// Do a regular HTTP post
			curl_easy_setopt(mCurl, CURLOPT_POST, 1L);

			if (!beaconData.empty())
			{
				if (mLogger->isDebugEnabled())
				{
					mLogger->debug("HTTPClient sendRequestInternal() - Beacon Payload: %s", beaconData.getStringData().c_str());
				}

				// Data to send is compressed => Compress the data
				Compressor::compressMemory(beaconData.getStringData().c_str(), beaconData.getStringLength(), mReadBuffer);
				mReadBufferPos = 0;
				curl_easy_setopt(mCurl, CURLOPT_READFUNCTION, readFunction);
				curl_easy_setopt(mCurl, CURLOPT_READDATA, this);
				curl_easy_setopt(mCurl, CURLOPT_POSTFIELDSIZE, mReadBuffer.size());
				list = curl_slist_append(list, "Content-Encoding: gzip");
			}
		}

		if (list != nullptr)
		{
			curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, list);
		}

		// Perform the request, res will get the return code
		CURLcode response = curl_easy_perform(mCurl);
		if (response == CURLE_OK)
		{
			// To retrieve the HTTP response code
			curl_easy_getinfo(mCurl, CURLINFO_RESPONSE_CODE, &httpCode);
		}
		else
		{
			// See https://curl.haxx.se/libcurl/c/libcurl-errors.html for a list of CURL error codes.
			mLogger->error("HTTPClient sendRequestInternal() - curl_easy_perform() failed on '%s': ErrorCode '%u', [%s]", url.getStringData().c_str(), response, curl_easy_strerror(response));
		}

		// Cleanup
		if (list != nullptr)
		{
			curl_slist_free_all(list);
			list = nullptr;
		}

		if (response == CURLE_OK)
		{
			curl_easy_cleanup(mCurl);
			mCurl = nullptr;

			// Check for success or error
			return handleResponse(requestType, httpCode, responseParser.getResponseBody(), responseParser.getResponseHeaders());
		}
		else
		{
			// For CURL related errors, we retry. Note that HTTP status codes >= 400 are returned with CURLE_OK.
			retryCount++;
			std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_SLEEP_TIME));
			curl_easy_reset(mCurl);
		}

	} while (retryCount < MAX_SEND_RETRIES);

	// Cleanup
	if (mCurl != nullptr)
	{
		curl_easy_cleanup(mCurl);
		mCurl = nullptr;
	}

	return HTTPClient::unknownErrorResponse(requestType);
}

std::shared_ptr<IStatusResponse> HTTPClient::handleResponse(RequestType requestType, int32_t httpCode, const std::string& response, const IStatusResponse::ResponseHeaders& responseHeaders)
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("HTTPClient handleResponse() - HTTP Response: %s", response.c_str());
		mLogger->debug("HTTPClient handleResponse() - HTTP Response Code: %u", (uint32_t)httpCode);
	}

	// check response code
	if (httpCode >= 400)
	{
		// erroneous response
		switch (requestType)
		{
		case RequestType::STATUS:
		case RequestType::NEW_SESSION: // FALLTHROUGH
		case RequestType::BEACON:      // FALLTHROUGH
			return StatusResponse::createErrorResponse(mLogger, httpCode, responseHeaders);
		default:
			return nullptr;
		}
	}
	else
	{
		try
		{
			auto responseAttributes = ResponseParser::parseResponse(core::UTF8String(response));
			return StatusResponse::createSuccessResponse(mLogger, responseAttributes, httpCode, responseHeaders);
		}
		catch (std::exception& e)
		{
			mLogger->error("HTTPClient handleResponse() Caught exception %s", e.what());
			return HTTPClient::unknownErrorResponse(requestType);
		}
	}
}

void HTTPClient::buildMonitorURL(core::UTF8String& monitorURL, const core::UTF8String& baseURL, const core::UTF8String& applicationID, uint32_t serverID)
{
	monitorURL.concatenate(baseURL);
	monitorURL.concatenate("?");
	monitorURL.concatenate(REQUEST_TYPE_MOBILE);

	appendQueryParam(monitorURL, QUERY_KEY_SERVER_ID, core::util::StringUtil::toInvariantString(serverID));
	appendQueryParam(monitorURL, QUERY_KEY_APPLICATION, applicationID);
	appendQueryParam(monitorURL, QUERY_KEY_VERSION, OPENKIT_VERSION);
	appendQueryParam(monitorURL, QUERY_KEY_PLATFORM_TYPE, PLATFORM_TYPE_OPENKIT);
	appendQueryParam(monitorURL, QUERY_KEY_AGENT_TECHNOLOGY_TYPE, AGENT_TECHNOLOGY_TYPE);
	appendQueryParam(monitorURL, QUERY_KEY_RESPONSE_TYPE, RESPONSE_TYPE_JSON);
}

void HTTPClient::buildNewSessionURL(core::UTF8String& newSessionURL, const core::UTF8String& baseURL, const core::UTF8String& applicationID, uint32_t serverID)
{
	buildMonitorURL(newSessionURL, baseURL, applicationID, serverID);
	appendQueryParam(newSessionURL, QUERY_KEY_NEW_SESSION, "1");
}

core::UTF8String HTTPClient::appendAdditionalQueryParameters(const core::UTF8String& baseUrl, const protocol::IAdditionalQueryParameters& parameters)
{
	auto newUrl = core::UTF8String(baseUrl);
	appendQueryParam(newUrl, QUERY_KEY_CONFIG_TIMESTAMP, core::util::StringUtil::toInvariantString(parameters.getConfigurationTimestamp()));

	return newUrl;
}

void HTTPClient::appendQueryParam(core::UTF8String& url, const char* key, const core::UTF8String& value)
{
	// converts the given value string to a URL encoded string
	url.concatenate("&");
	url.concatenate(core::util::URLEncoding::urlencode(key, { '_' }));
	url.concatenate("=");
	url.concatenate(core::util::URLEncoding::urlencode(value, { '_' }));
}

std::shared_ptr<IStatusResponse> HTTPClient::unknownErrorResponse(RequestType requestType)
{
	switch (requestType) {
	case RequestType::STATUS:
	case RequestType::BEACON: // fallthrough
	case RequestType::NEW_SESSION: // fallthrough
		return StatusResponse::createErrorResponse(mLogger, std::numeric_limits<int32_t>::max());
	default:
		// should not be reached
		return nullptr;
	}
}
