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

#ifndef _CORE_NULLWEBREQUESTTRACER_H
#define _CORE_NULLWEBREQUESTTRACER_H

#include "api/IWebRequestTracer.h"

namespace core
{

	///
	/// This class is returned as WebRequestTracer by @ref OpenKit::createSession(const char*) when the @ref OpenKit::shutdown()
	/// has been called before.
	///
	class NullWebRequestTracer : public api::IWebRequestTracer, public std::enable_shared_from_this<NullWebRequestTracer>
	{
	public:

		const char* getTag() const override
		{
			return emptyString;
		}

		virtual std::shared_ptr<api::IWebRequestTracer> setResponseCode(int32_t /*responseCode*/) override
		{
			return shared_from_this();
		}

		virtual std::shared_ptr<api::IWebRequestTracer> setBytesSent(int32_t /*bytesSent*/) override
		{
			return shared_from_this();
		}

		virtual std::shared_ptr<api::IWebRequestTracer> setBytesReceived(int32_t /*bytesReceived*/) override
		{
			return shared_from_this();
		}

		virtual std::shared_ptr<api::IWebRequestTracer> start() override
		{
			return shared_from_this();
		}

		virtual void stop() override
		{
			// intentionally left empty, due to NullObject pattern
		}

		virtual bool isNullObject() const override
		{
			return true;
		}

	private:
		const char* emptyString = "";
	};
}

#endif