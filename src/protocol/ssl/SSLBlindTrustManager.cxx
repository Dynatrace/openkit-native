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

#include "protocol/ssl/SSLBlindTrustManager.h"

#include <iostream>

using namespace protocol;

SSLBlindTrustManager::SSLBlindTrustManager()
{
	std::cerr << "###########################################################" << std::endl;
	std::cerr << "# WARNING: YOU ARE BYPASSING SSL CERTIFICATE VALIDATION!! #" << std::endl;
	std::cerr << "#                 USE AT YOUR OWN RISK!!                  #" << std::endl;
	std::cerr << "###########################################################" << std::endl;
}

SSLBlindTrustManager::~SSLBlindTrustManager()
{
}

void SSLBlindTrustManager::applyTrustManager(CURL* curl)
{
	// disable verification of the peer's TSL/SSL certificate
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

	// disable verification of the TSL/SSL certificate name against host
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
}

