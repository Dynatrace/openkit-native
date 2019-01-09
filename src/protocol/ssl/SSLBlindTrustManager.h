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

#ifndef _PROTOCOL_SSL_BLINDTRUSTMANAGER_H
#define _PROTOCOL_SSL_BLINDTRUSTMANAGER_H

#include "OpenKit/ISSLTrustManager.h"

#include "curl/curl.h"

namespace protocol
{
	///
	/// Implementation of the @c ISSLTrustManager blindly trusting every certificate and every host.
	///
	/// This class is intended to be used only during development phase. Since local
	/// development environments use self - signed certificates only.
	/// This implementation disables any certificate validation and hostname validation.
	///
	/// NOTE: DO NOT USE THIS IN PRODUCTION!!
	///
	class SSLBlindTrustManager : public openkit::ISSLTrustManager
	{
	public:

		///
		/// Instantiates a blind trusting SSL Trust Manager which blindly trusts all certificates.
		///
		SSLBlindTrustManager();

		///
		/// Delete the copy constructor
		///
		SSLBlindTrustManager(const SSLBlindTrustManager&) = delete;

		///
		/// Delete the assignment operator
		///
		SSLBlindTrustManager& operator = (const SSLBlindTrustManager &) = delete;

		virtual void applyTrustManager(CURL* curl) override;

	};

}

#endif