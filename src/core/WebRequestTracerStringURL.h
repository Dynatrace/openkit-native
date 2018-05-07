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

#ifndef _CORE_WEBREQUESTTRACERSTRINGURL_H
#define _CORE_WEBREQUESTTRACERSTRINGURL_H

#include "core/WebRequestTracerBase.h"

namespace core
{

	///
	/// Setting the Dynatrace tag to the @ref OpenKitConstants.WEBREQUEST_TAG_HEADER 
	/// HTTP header has to be done manually by the user.
	/// Inherited class of @ref WebRequestTracerBase which can be used for 
	/// tracing and timing of a web request handled by any 3rd party HTTP Client.
	///
	class WebRequestTracerStringURL : public WebRequestTracerBase
	{
	public:

		///
		/// Constructor taking an Action
		/// @param[in] logger to write traces to
		/// @param[in] beacon @ref Beacon used to serialize the WebRequestTracer
		/// @param[in] action parent of the WebRequestTracer
		/// @param[in] url url of the web request
		///
		WebRequestTracerStringURL(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<protocol::Beacon> beacon, int32_t parentActionID, const UTF8String& url);

		///
		/// Test if given {@c url} contains a valid URL scheme according to RFC3986.
		/// @param[in] The url to validate
		/// @return {@code true} on success, {@code false} otherwhise.
		///
		static bool isValidURLScheme(const UTF8String& url);
	};
}

#endif