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
#ifndef _API_C_EXTERNALTRUSTMANAGER_H
#define _API_C_EXTERNALTRUSTMANAGER_H

#include "OpenKit/ISSLTrustManager.h"
#include "api-c/OpenKit-c.h"

#include "curl/curl.h"

#include <cstdarg>

namespace apic
{
	/// The OpenKit comes with a default @c BlindTrustManager, trusting every certificate and every host.
	/// This implementation is intended to be used only during development phase. Since local development
	/// environments use self-signed certificates only. This implementation disables any certificate validation
	/// and hostname validation.
	/// To override the @c BlindTrustManager for production, the @c CustomTrustManager acts as the glue:
	/// On the one hand it implements the ISSLTrustManager (C++) interface on the other hand it calls the
	/// user provided function pointer to apply the trust configuration on the CURL handle.
	class CustomTrustManager : public openkit::ISSLTrustManager
	{
	public:
		///
		/// Constructor
		///
		CustomTrustManager(applyTrustManagerFunc applyTrustManagerFunc);
		
		///
		/// Destructor
		///
		virtual ~CustomTrustManager() {}

		virtual void applyTrustManager(CURL* curl) override;

	private:
		/// Function pointer of the apply trust configuration function
		applyTrustManagerFunc mApplyTrustManagerFunc;
	};
}

#endif
