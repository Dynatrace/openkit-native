/**
* Copyright 2018-2019 Dynatrace LLC
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

#include "protocol/ssl/SSLStrictTrustManager.h"

using namespace protocol;

SSLStrictTrustManager::SSLStrictTrustManager()
{
}

void SSLStrictTrustManager::applyTrustManager(CURL* curl)
{
	// Sanity
	if (curl == nullptr)
	{
		return;
	}

	// verifies the authenticity of the peer's TSL/SSL certificate
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

	// verify the server's TSL/SSL certificate's claimed identity
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
}

