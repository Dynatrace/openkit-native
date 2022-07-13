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


#ifndef _OPENKIT_IHTTPRESPONSEINTERCEPTOR_H
#define _OPENKIT_IHTTPRESPONSEINTERCEPTOR_H

#include "OpenKit/OpenKitExports.h"
#include "IHttpResponse.h"

namespace openkit
{
	///
	/// An interface allowing to intercept an HTTP response from Dynatrace backends.
	///
	/// @par
	/// This interceptor is only applied to HTTP responses coming from Dynatrace backend requests.
	///
	class OPENKIT_EXPORT IHttpResponseInterceptor
	{
	public:

		///
		/// Destructor
		///
		virtual ~IHttpResponseInterceptor() = default;

		///
		/// Intercept the HTTP response from Dynatrace backend.
		/// 
		/// @par
		/// This method allows the implementor to read custom HTTP response headers.
		/// 
		/// @param httpResponse The HTTP response from Dynatrace backend.
		///
		virtual void intercept(const IHttpResponse& httpResponse) = 0;
	};
}

#endif
