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


#ifndef _OPENKIT_IHTTPREQUEST_H
#define _OPENKIT_IHTTPREQUEST_H

#include "OpenKitExports.h"

#include <string>

namespace openkit
{
	///
	/// Class representing an HTTP request.
	/// 
	/// @par
	/// Contrary to other other OpenKit APIs the IHttpRequest methods
	/// mainly work with STL strings and expect them to be ASCII encoded.
	///
	class OPENKIT_EXPORT IHttpRequest
	{
	public:

		///
		/// Destructor
		///
		virtual ~IHttpRequest() = default;

		///
		/// Get the request URI.
		///
		/// @return String containing the request URI.
		///
		virtual const std::string& getUri() const = 0;

		///
		/// Get the request method.
		///
		/// @return String, which is either @c "GET" or @c "POST".
		///
		virtual const std::string& getMethod() const = 0;

		///
		/// Tests if the request header is existent in the HTTP request.
		/// 
		/// @param name Header's name to check for existence.
		///
		/// @retval @c true if header exists
		/// @retval @c false if header does not exist
		///
		virtual bool containsHeader(const std::string& name) const = 0;

		///
		/// Gets the header's value.
		/// 
		/// @par
		/// Use IHttpRequest::containsHeader(const std::string&) to check
		/// if the header exists.
		/// If this method is called with a header name that does not exist
		/// an empty string is returned.
		/// 
		/// @param name Header's name for which to get the value.
		/// 
		/// @return Header values for given header name.
		///
		virtual const std::string& getHeader(const std::string& name) const = 0;

		///
		/// Sets an HTTP header or overwrites an existing HTTP header with new value.
		/// 
		/// @par
		/// For compatibility reasons the following headers are restricted and cannot be set.
		/// - @c Access-Control-Request-Headers
		/// - @c Access-Control-Request-Method
		/// - @c Connection
		/// - @c Content-Length
		/// - @c Content-Transfer-Encoding
		/// - @c Host
		/// - @c Keep-Alive
		/// - @c Origin
		/// - @c Trailer
		/// - @c Transfer-Encoding
		/// - @c Upgrade
		/// - @c Via
		///
		/// @param name The header name to set.
		/// @param value The header's value.
		///
		virtual void setHeader(const std::string& name, const std::string& value) = 0;
	};
}

#endif
