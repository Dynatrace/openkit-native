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
#include "protocol/HTTPClient.h"

#include <cstdint>
#include <gtest/gtest.h>

using namespace core;
using namespace protocol;
using namespace configuration;

class HTTPClientTest : public testing::Test
{
public:
	void SetUp()
	{

	}

	void TearDown()
	{

	}
};

#if 0 /* Tests ignored since the tests require a working Internet connection. */

TEST_F(HTTPClientTest, SendStatusRequestToInvalidUrl)
{
	UTF8String beaconURL = UTF8String("http://example.com");
	uint32_t serverID = 1;
	UTF8String applicationID = UTF8String("appID");

	std::shared_ptr<HTTPClientConfiguration> httpClientConfig = std::shared_ptr<HTTPClientConfiguration>(new HTTPClientConfiguration(beaconURL, serverID, applicationID));
	std::shared_ptr<HTTPClient> httpClient = std::shared_ptr<HTTPClient>(new HTTPClient(httpClientConfig));

	std::unique_ptr<StatusResponse> statusResponse = httpClient->sendStatusRequest();
	EXPECT_EQ(nullptr, statusResponse);	// since example.com doesn't return a proper beacon response
}

TEST_F(HTTPClientTest, SendStatusRequestToValidUrl)
{
	UTF8String beaconURL = UTF8String("https://bf79563wis.bf-dev.dynatracelabs.com/mbeacon/type=m&srvid=5&app=56236c21-40b1-498f-aed9-65bbb75580d2&va=7.0.0000&pt=1&tt=okc");
	uint32_t serverID = 5;
	UTF8String applicationID = UTF8String("d99632e3-7337-4814-98d8-2dc574cc4dbf");

	std::shared_ptr<HTTPClientConfiguration> httpClientConfig = std::shared_ptr<HTTPClientConfiguration>(new HTTPClientConfiguration(beaconURL, serverID, applicationID));
	std::shared_ptr<HTTPClient> httpClient = std::shared_ptr<HTTPClient>(new HTTPClient(httpClientConfig));

	std::unique_ptr<StatusResponse> statusResponse = httpClient->sendStatusRequest();
	EXPECT_NE(nullptr, statusResponse);
	EXPECT_NE(200, statusResponse->getResponseCode());
}

TEST_F(HTTPClientTest, SendBeaconRequestToValidUrlWithData)
{
	UTF8String clientIPAddress = UTF8String("127.0.0.1");
	const char* ascii_json_data =
		"{"
		"	\"data\": {"
		"	\"name\": \"Name\","
		"		\"number\" : 4,"
		"		\"text\" : \"Some data\""
		"	}"
		"}";
	UTF8String beaconURL = UTF8String("https://bf79563wis.bf-dev.dynatracelabs.com/mbeacon/type=m&srvid=5&app=56236c21-40b1-498f-aed9-65bbb75580d2&va=7.0.0000&pt=1&tt=okc");
	uint32_t serverID = 5;
	UTF8String applicationID = UTF8String("d99632e3-7337-4814-98d8-2dc574cc4dbf");

	std::shared_ptr<HTTPClientConfiguration> httpClientConfig = std::shared_ptr<HTTPClientConfiguration>(new HTTPClientConfiguration(beaconURL, serverID, applicationID));
	std::shared_ptr<HTTPClient> httpClient = std::shared_ptr<HTTPClient>(new HTTPClient(httpClientConfig));

	std::unique_ptr<StatusResponse> statusResponse = httpClient->sendBeaconRequest(clientIPAddress, ascii_json_data, strlen(ascii_json_data) + 1);
	EXPECT_NE(nullptr, statusResponse);
	EXPECT_NE(200, statusResponse->getResponseCode());
}
#endif