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
	/// The OpenKit comes with a default logger printing trace statements to stdout. 
	/// To override the default logger with a user provided logger from the C binding wrapper,
	/// the @c CustomLogger acts as the glue:
	/// On the one hand it implements the ILogger (C++) interface on the other hand it calls the
	/// user provided function pointers to check if to log and to perform the log.
	class CustomTrustManager : public openkit::ISSLTrustManager
	{
	public:
		///
		/// Constructor
		///
		CustomTrustManager();
		
		///
		/// Destructor
		///
		virtual ~CustomTrustManager() {}

	private:
		///
		/// Does the actual logging to the provided function pointer
		///
		void applyTrustManager(CURL* curl);

	};
}

#endif
