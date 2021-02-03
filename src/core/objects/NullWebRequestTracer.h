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

#ifndef _CORE_OBJECTS_NULLWEBREQUESTTRACER_H
#define _CORE_OBJECTS_NULLWEBREQUESTTRACER_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif


#include "OpenKit/IWebRequestTracer.h"

namespace core
{
	namespace objects
	{
		///
		/// This class is returned as WebRequestTracer by @ref openkit::IOpenKit::createSession(const char*) when the
		/// @ref openkit::IOpenKit::shutdown() has been called before.
		///
		class NullWebRequestTracer
			: public openkit::IWebRequestTracer
			, public std::enable_shared_from_this<NullWebRequestTracer>
		{
		public:

			static const std::shared_ptr<NullWebRequestTracer> instance();

			const char* getTag() const override;

			///
			/// @deprecated use stop(int32_t) instead
			///
			OPENKIT_DEPRECATED
			std::shared_ptr<openkit::IWebRequestTracer> setResponseCode(int32_t /*responseCode*/) override;

			std::shared_ptr<openkit::IWebRequestTracer> setBytesSent(int32_t /*bytesSent*/) override;

			std::shared_ptr<openkit::IWebRequestTracer> setBytesReceived(int32_t /*bytesReceived*/) override;

			std::shared_ptr<openkit::IWebRequestTracer> start() override;

			///
			/// @deprecated use stop(int32_t) instead
			///
			OPENKIT_DEPRECATED
			void stop() override;

			void stop(int32_t /*responseCode*/) override;

		private:
			const char* emptyString = "";
		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
