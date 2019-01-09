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

#ifndef _PROTOCOL_SSL_STRICTTRUSTMANAGER_H
#define _PROTOCOL_SSL_STRICTTRUSTMANAGER_H

#include "OpenKit/ISSLTrustManager.h"

#include "curl/curl.h"

namespace protocol
{
	///
	/// Implementation of @c ISSLTrustManager trusting only valid certificates.
	///
	/// This is the default strategy in SSL certificate validation and should NOT be changed.
	///
	class SSLStrictTrustManager : public openkit::ISSLTrustManager
	{
	public:

		///
		/// Instantiates a strict SSL Trust Manager which verifies the peer's SSL certificate and which verifies that the server host matches the certificate's name
		///
		SSLStrictTrustManager();

		///
		/// Delete the copy constructor
		///
		SSLStrictTrustManager(const SSLStrictTrustManager&) = delete;

		///
		/// Delete the assignment operator
		///
		SSLStrictTrustManager& operator = (const SSLStrictTrustManager &) = delete;

		virtual void applyTrustManager(CURL* curl) override;

	};

}

#endif