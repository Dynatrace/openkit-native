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

#ifndef _PROTOCOL_SSL_ISSLTRUSTMANAGER_H
#define _PROTOCOL_SSL_ISSLTRUSTMANAGER_H

#include "OpenKit_export.h"

#include "curl/curl.h"

namespace openkit
{
	///
	/// Interface to provide a user-defined trust manager to the configuration.
	/// When OpenKit connects to a server with self-signed SSL/TLS certificates (e.g. AppMon) then
	/// an implementation of this interface is required to verify the certificate.
	///
	class OPENKIT_EXPORT ISSLTrustManager
	{
	public:

		///
		/// Destructor
		///
		virtual ~ISSLTrustManager() {}

		///
		/// Apply the trust configuration on the provided curl handle
		///
		virtual void applyTrustManager(CURL* curl) = 0;

	};
}
#endif