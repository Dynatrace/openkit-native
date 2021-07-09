/**
 * Copyright 2018-2021 Dynatrace LLC
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
#ifndef _OPENKIT_IHTTPREQUESTINTERCEPTOR_H
#define _OPENKIT_IHTTPREQUESTINTERCEPTOR_H

#include "OpenKitExports.h"
#include "IHttpRequest.h"

namespace openkit
{
	///
	/// An interface allowing to intercept an HTTP request, before it is sent to the backend system.
	///
	/// @par
	/// This interceptor is only applied to HTTP requests which are sent to Dynatrace backends.
	///
	class OPENKIT_EXPORT IHttpRequestInterceptor
	{
	public:

		///
		/// Destructor
		///
		virtual ~IHttpRequestInterceptor() = default;

		///
		/// Intercept the HTTP request and manipulate it.
		/// 
		/// @par
		/// Currently it's only possible to set custom HTTP headers.
		/// 
		/// @param httpRequest The HTTP request to Dynatrace backend.
		///
		virtual void intercept(IHttpRequest& httpRequest) = 0;
	};
}

#endif
