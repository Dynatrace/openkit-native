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


#ifndef _OPENKIT_IHTTPRESPONSE_H
#define _OPENKIT_IHTTPRESPONSE_H

#include "OpenKitExports.h"

#include <string>
#include <list>

namespace openkit
{
	///
	/// Class representing an HTTP response.
	/// 
	/// @par
	/// Contrary to other other OpenKit APIs the IHttpResponse methods
	/// work with STL strings and expect them to be ASCII encoded.
	///
	class OPENKIT_EXPORT IHttpResponse
	{
	public:

		///
		/// Destructor
		///
		virtual ~IHttpResponse() = default;

		///
		/// Get the request URI.
		///
		/// @return Null-terminated string containing the request URI.
		///
		virtual const std::string& getRequestUri() const = 0;

		///
		/// Get the request method.
		///
		/// @return Null-terminated string, which is either @c "GET" or @c "POST".
		///
		virtual const std::string& getRequestMethod() const = 0;

		///
		/// Gets the HTTP response code.
		/// 
		/// @return The 3 digit status code, or a negative value in case of a parsing error.
		///
		virtual int32_t getStatusCode() const = 0;

		///
		/// Gets the reason phrase returned by the HTTP server.
		/// 
		/// @return Reason phrase or an empty string, if the server did not set it.
		///
		virtual const std::string& getReasonPhrase() const = 0;

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
		/// Gets the header's values.
		/// 
		/// @par
		/// For headers with multiple values, where values are delimited with ","
		/// this method will return a list containing one element.
		/// For instance
		/// @code{.unparsed}
		/// Accept: text/plain, text/html
		/// @endcode
		/// will give @c "text/plain, text/html" as the only entry for "Accept" in the returned list.
		/// If the header occurs multiple times, like the @c Set-Cookie, the list contains
		/// all occurrences in the order they appear in the response.
		/// 
		/// @param name Header's name for which to get the values.
		/// 
		/// @return Header values for given header name, or empty list if no such header exists.
		///
		virtual const std::list<std::string>& getHeader(const std::string& name) const = 0;
	};
}

#endif
