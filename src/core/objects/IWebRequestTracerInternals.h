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

#ifndef _CORE_OBJECTS_IWEBREQUESTTRACERINTERNALS_H
#define _CORE_OBJECTS_IWEBREQUESTTRACERINTERNALS_H

#include "OpenKit/IWebRequestTracer.h"
#include "core/UTF8String.h"
#include "core/objects/IOpenKitComposite.h"
#include "core/objects/IOpenKitObject.h"

namespace core
{
	namespace objects
	{
		class IWebRequestTracerInternals
			: public openkit::IWebRequestTracer
			, public core::objects::IOpenKitObject
		{
		public:

			~IWebRequestTracerInternals() override = default;

			const char* getTag() const override = 0;

			///
			/// @deprecated use stop(int32_t) instead
			///
			OPENKIT_DEPRECATED
			std::shared_ptr<IWebRequestTracer> setResponseCode(int32_t responseCode) override = 0;

			std::shared_ptr<IWebRequestTracer> setBytesSent(int32_t bytesSent) override = 0;

			std::shared_ptr<IWebRequestTracer> setBytesReceived(int32_t bytesReceived) override = 0;

			std::shared_ptr<IWebRequestTracer> start() override = 0;

			///
			/// @deprectaed use stop(int32_t) instead
			///
			OPENKIT_DEPRECATED
			void stop() override = 0;

			void stop(int32_t responseCode) override = 0;

			void close() override = 0;

			///
			/// Returns the target URL of the web request
			/// @returns target URL of the web request
			///
			virtual const core::UTF8String getURL() const = 0;

			///
			/// Returns the response code of the web request
			/// @returns response code of the web request
			///
			virtual int32_t getResponseCode() const = 0;

			///
			/// Returns the start time of the web request
			/// @returns start time of the web request
			///
			virtual int64_t getStartTime() const = 0;

			///
			/// Returns the end time of the web request
			/// @returns end time of the web request
			///
			virtual int64_t getEndTime() const = 0;

			///
			/// Returns the start sequence number
			/// @returns start sequence number of the web request
			///
			virtual int32_t getStartSequenceNo() const = 0;

			///
			/// Returns the end sequence number
			/// @returns end sequence number of the web request
			///
			virtual int32_t getEndSequenceNo() const = 0;

			///
			/// Returns the number of bytes sent
			/// @returns amount of bytes sent
			///
			virtual int32_t getBytesSent() const = 0;

			///
			/// Returns the number of bytes received
			/// @returns amount of bytes received
			///
			virtual int32_t getBytesReceived() const = 0;

			///
			/// Returns a flag if stop() has already been called
			/// @returns @c true if the web request tracer has been stopped already, @c false otherwise
			///
			virtual bool isStopped() const = 0;

			///
			/// Returns the parent object of this web request tracer.
			///
			virtual std::shared_ptr<IOpenKitComposite> getParent() const = 0;
		};
	}
}

#endif
