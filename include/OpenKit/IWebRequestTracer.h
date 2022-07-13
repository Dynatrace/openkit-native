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

#ifndef _OPENKIT_WEBREQUESTTRACER_H
#define _OPENKIT_WEBREQUESTTRACER_H

#include "OpenKit/OpenKitExports.h"

#include <cstdint>
#include <memory>

namespace openkit
{
	///
	/// This interface allows tracing and timing of a web request.
	///
	class OPENKIT_EXPORT IWebRequestTracer
	{
	public:

		virtual ~IWebRequestTracer() {}

		///
		/// Returns the Dynatrace tag which has to be set manually as Dynatrace HTTP header
		/// ( @ref openkit::OpenKitConstants::WEBREQUEST_TAG_HEADER).
		/// This is only necessary for tracing web requests via 3rd party HTTP clients.
		///
		/// @returns the Dynatrace tag to be set as HTTP header value or an empty String if capture is off
		///
		virtual const char* getTag() const = 0;

		///
		/// Sets the response code of this web request. Has to be called before @ref openkit::IWebRequestTracer::stop().
		///
		/// @param[in] responseCode response code of this web request
		/// @returns pointer to this WebRequestTracer instance that can be used for a fluent-API call
		/// @deprecated use stop(int32_t) instead
		///
		OPENKIT_DEPRECATED
		virtual std::shared_ptr<IWebRequestTracer> setResponseCode(int32_t responseCode) = 0;

		///
		/// Sets the amount of sent data of this web request. Has to be called before @ref openkit::IWebRequestTracer::stop().
		///
		/// @param[in] bytesSent number of bytes sent
		/// @returns pointer to this WebRequestTracer instance that can be used for a fluent-API call
		///
		virtual std::shared_ptr<IWebRequestTracer> setBytesSent(int32_t bytesSent) = 0;

		///
		/// Sets the amount of received data of this web request. Has to be called before @ref openkit::IWebRequestTracer::stop().
		///
		/// @param[in] bytesReceived number of bytes received
		/// @returns pointer to this WebRequestTracer instance that can be used for a fluent-API call
		///
		virtual std::shared_ptr<IWebRequestTracer> setBytesReceived(int32_t bytesReceived) = 0;

		///
		/// Starts the web request timing. Should be called when the web request is initiated.
		///
		virtual std::shared_ptr<IWebRequestTracer> start() = 0;

		///
		/// Stops the web request timing. Should be called when the web request is finished.
		/// @deprectated use stop(int32_t) instead
		///
		OPENKIT_DEPRECATED
		virtual void stop() = 0;

		///
		/// Stops the web request timing with the given response code. Should be called when the web request is finished.
		/// @param[in] responseCode response code of this web request
		///
		virtual void stop(int32_t responseCode) = 0;
	};
}
#endif
