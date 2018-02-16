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

// connection constants
constexpr int MAX_SEND_RETRIES = 3;		// max number of retries of the HTTP GET or POST operation
constexpr int RETRY_SLEEP_TIME = 200;	// retry sleep time in ms
constexpr long CONNECT_TIMEOUT = 5;		// Time-out connect operations after this amount of seconds
constexpr long READ_TIMEOUT = 30;		// Time-out the read operation after this amount of seconds

#include "HTTPClient.h"
#include "ProtocolConstants.h"
#include "core/util/Compressor.h"

using namespace protocol;
using namespace base::util;

HTTPClient::HTTPClient(std::shared_ptr<configuration::HTTPClientConfiguration> configuration)
	: mServerID(configuration->getServerID())
{
	/* build the beacon URLs */
	buildMonitorURL(mMonitorURL, configuration->getBaseURL(), configuration->getApplicationID(), mServerID);
	buildTimeSyncURL(mTimeSyncURL, configuration->getBaseURL());
	// TODO: sslTrustManager = configuration.getSSLTrustManager();

	/* set up the program environment that libcurl needs. In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session - get the curl handle */
	mCurl = curl_easy_init();

	if (!mCurl)
	{
		/* Abort and cleanup if CURL cannot be initialized */
		fprintf(stderr, "curl_easy_init() failed\n"); // TODO: Add support for a proper logger
		curl_global_cleanup();
	}
}

HTTPClient::~HTTPClient()
{
	if (!mCurl)
	{
		curl_easy_cleanup(mCurl);
		mCurl = nullptr;
	}

	curl_global_cleanup();
}

std::unique_ptr<StatusResponse> HTTPClient::sendStatusRequest()
{
	auto response = sendRequestInternal(RequestType::STATUS, mMonitorURL, "", nullptr, 0, HttpMethod::GET);
	if (response)
	{
		return std::unique_ptr<StatusResponse>(reinterpret_cast<StatusResponse*>(response.release()));
	}
	return nullptr;
}

std::unique_ptr<StatusResponse> HTTPClient::sendBeaconRequest(const core::UTF8String& clientIPAddress, const void* data, size_t dataSize)
{
	auto response = sendRequestInternal(RequestType::BEACON, mMonitorURL, clientIPAddress, data, dataSize, HttpMethod::POST);
	if (response)
	{
		return std::unique_ptr<StatusResponse>(reinterpret_cast<StatusResponse*>(response.release()));
	}
	return nullptr;
}

std::unique_ptr<TimeSyncResponse> HTTPClient::sendTimeSyncRequest()
{
	auto response = sendRequestInternal(RequestType::TIMESYNC, mTimeSyncURL, "", nullptr, 0, HttpMethod::GET);
	if (response)
	{
		return std::unique_ptr<TimeSyncResponse>(reinterpret_cast<TimeSyncResponse*>(response.release()));
	}
	return nullptr;
}

typedef struct
{
	void *data;
	size_t body_size;
	size_t body_pos;
} Postdata;

///
/// Local callback function for reading data to upload (=the data in a POST request).
/// @param[in,out] ptr where the data to POST is written
/// @param[in] size of the data
/// @param[in] nmemb number of data (size of the written data = size * nmemb)
/// @param[in] stream the user data to upload is read from there
/// @return the size of the delievered data
///
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	if (stream)
	{
		Postdata *ud = (Postdata*)stream;
		size_t available = (ud->body_size - ud->body_pos);

		if (available > 0)
		{
			size_t written = min(size * nmemb, available);
			memcpy(ptr, ((char*)(ud->data)) + ud->body_pos, written);
			ud->body_pos += written;
			return written;
		}
	}

	return 0;
}

///
/// Local callback function for writing received data (=the response).
/// @param[in] ptr to the delivered data
/// @param[in] size of the data
/// @param[in] nmemb number of data (size of the delivered data = size * nmemb)
/// @param[in,out] data to write the received data to
/// @return the size of the delievered data
///
static size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data)
{
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

std::unique_ptr<Response> HTTPClient::sendRequestInternal(const HTTPClient::RequestType requestType, const core::UTF8String& url, const core::UTF8String& clientIPAddress, const void* inData, size_t inDataSize, const HTTPClient::HttpMethod method)
{
	// Sanity
	if (!mCurl)
	{
		return nullptr;
	}

	long httpCode = 0;
	int retryCnt = 0;
	do
	{
		/* Set the connection parameters (URL, timeouts, etc.) */
		curl_easy_setopt(mCurl, CURLOPT_URL, url.getStringData().c_str());
		curl_easy_setopt(mCurl, CURLOPT_CONNECTTIMEOUT, CONNECT_TIMEOUT);
		curl_easy_setopt(mCurl, CURLOPT_TIMEOUT, READ_TIMEOUT);
		/* allow servers to send compressed data*/
		curl_easy_setopt(mCurl, CURLOPT_ACCEPT_ENCODING, "");

		/** To retrieve the response */
		std::string responseBuffer;
		curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, writeFunction);
		curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &responseBuffer);

		/* Set the custom HTTP header with the client IP address, if provided */
		struct curl_slist *list = NULL;
		if (!clientIPAddress.empty())
		{
			core::UTF8String xClientId("X-Client-IP: ");
			xClientId.concatenate(clientIPAddress);
			list = curl_slist_append(list, xClientId.getStringData().c_str());
			if (list != NULL)
			{
				curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, list);
			}
		}

		if (method == POST)
		{
			/** Do a regular HTTP post */
			curl_easy_setopt(mCurl, CURLOPT_POST, 1L);

			if (inData != nullptr && inDataSize > 0)
			{
				/* Data to send is compressed => Compress the data */
				std::vector<unsigned char> compressedBuffer;
				Compressor::compressMemory(inData, inDataSize, compressedBuffer);
				Postdata writer;
				writer.body_pos = 0;
				writer.data = &(compressedBuffer[0]);
				writer.body_size = compressedBuffer.size();
				curl_easy_setopt(mCurl, CURLOPT_READFUNCTION, read_callback);
				curl_easy_setopt(mCurl, CURLOPT_READDATA, &writer);
			}
		}

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(mCurl);
		if (res == CURLE_OK)
		{
			/** To retrieve the HTTP response code */
			curl_easy_getinfo(mCurl, CURLINFO_RESPONSE_CODE, &httpCode);
		}
		else
		{
			/* See https://curl.haxx.se/libcurl/c/libcurl-errors.html for a list of CURL error codes. */
			fprintf(stderr, "curl_easy_perform() failed on '%s': ErrorCode '%u', [%s]\n", url.getStringData().c_str(), res, curl_easy_strerror(res)); // TODO: Add support for a proper logger
		}

		/** Cleanup */
		if (list != nullptr)
		{
			curl_slist_free_all(list);
			list = nullptr;
		}

		if (res == CURLE_OK)
		{
			curl_easy_cleanup(mCurl);
			mCurl = nullptr;

			/* Check for success or error */
			return handleResponse(httpCode, responseBuffer);
		}
		else
		{
			/* For CURL related errors, we retry. Note that HTTP status codes >= 400 are returned with CURLE_OK. */
			retryCnt++;
			Sleep(RETRY_SLEEP_TIME);
			curl_easy_reset(mCurl);
		}

	} while (retryCnt < MAX_SEND_RETRIES);

	/* Cleanup */
	if (mCurl != nullptr)
	{
		curl_easy_cleanup(mCurl);
		mCurl = nullptr;
	}
	return nullptr;
}

std::unique_ptr<Response> HTTPClient::handleResponse(long httpCode, const std::string& response)
{
	// check response code
	if (httpCode >= 400)
	{
		// process error
		// TODO: See java version of using the logger

		// read error response
		return nullptr;
	}
	else
	{
		// process status response
		if (response.find(REQUEST_TYPE_TIMESYNC) == 0)
		{
			// TODO: Change to TimeSyncResponse
			return std::unique_ptr<StatusResponse>(new StatusResponse(core::UTF8String(response.c_str()), (uint32_t)httpCode));
		}
		else if (response.find(REQUEST_TYPE_MOBILE) == 0)
		{
			return std::unique_ptr<StatusResponse>(new StatusResponse(core::UTF8String(response.c_str()), (uint32_t)httpCode));
		}
		else
		{
			return nullptr;
		}
	}
}

///
/// Build URL used for status check and beacon send requests
/// @param[in,out] monitorURL the url to build
/// @param[in] baseURL the beacon base url without the query string
/// @param[in] applicationID
/// @param[in] serverID
///
void HTTPClient::buildMonitorURL(core::UTF8String& monitorURL, const core::UTF8String& baseURL, const core::UTF8String& applicationID, uint32_t serverID)
{
	monitorURL.concatenate(baseURL);
	monitorURL.concatenate("?");
	monitorURL.concatenate(REQUEST_TYPE_MOBILE);

	appendQueryParam(monitorURL, QUERY_KEY_SERVER_ID, std::to_string(serverID).c_str());
	appendQueryParam(monitorURL, QUERY_KEY_APPLICATION, applicationID.getStringData().c_str());
	appendQueryParam(monitorURL, QUERY_KEY_VERSION, OPENKIT_VERSION);
	appendQueryParam(monitorURL, QUERY_KEY_PLATFORM_TYPE, PLATFORM_TYPE_OPENKIT);
	appendQueryParam(monitorURL, QUERY_KEY_AGENT_TECHNOLOGY_TYPE, AGENT_TECHNOLOGY_TYPE);
}

void HTTPClient::buildTimeSyncURL(core::UTF8String& monitorURL, const core::UTF8String& baseURL)
{
	monitorURL.concatenate(baseURL);
	monitorURL.concatenate("?");
	monitorURL.concatenate(REQUEST_TYPE_TIMESYNC);
}

void HTTPClient::appendQueryParam(core::UTF8String& url, const char* key, const char* value)
{
	/* converts the given value string to a URL encoded string */
	char *encodedValue = curl_easy_escape(mCurl, value, 0);
	if (encodedValue)
	{
		url.concatenate("&");
		url.concatenate(key);
		url.concatenate("=");
		url.concatenate(encodedValue);
		curl_free(encodedValue);
	}
}

