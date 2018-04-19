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

#ifndef _TEST_PROVIDERS_MOCKHTTPCLIENTPROVIDER_H
#define _TEST_PROVIDERS_MOCKHTTPCLIENTPROVIDER_H

#include "providers/IHTTPClientProvider.h"

namespace test {
	class MockHTTPClientProvider : public providers::IHTTPClientProvider
	{
	public:
		///
		/// Default constructor
		///
		MockHTTPClientProvider()
		{

		}

		virtual ~MockHTTPClientProvider() {}

		MOCK_METHOD2(createClient, std::shared_ptr<protocol::IHTTPClient>(std::shared_ptr<openkit::ILogger>, std::shared_ptr<configuration::HTTPClientConfiguration>));

	private:
		std::ostringstream devNull;
	};
}

#endif

