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

#ifndef _TEST_PROVIDERS_TESTHTTPCLIENTPROVIDER_H
#define _TEST_PROVIDERS_TESTHTTPCLIENTPROVIDER_H

#include "providers/DefaultHTTPClientProvider.h"
#include "../protocol/TestHTTPClient.h"

namespace test {
	class TestHTTPClientProvider : public providers::DefaultHTTPClientProvider
	{
	public:
		std::unique_ptr<protocol::HTTPClient> createClient(std::shared_ptr<configuration::HTTPClientConfiguration> configuration)
		{
			return std::unique_ptr<protocol::HTTPClient>(new test::TestHTTPClient(configuration));
		}
	};
}

#endif