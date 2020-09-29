/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _TEST_CORE_OBJECTS_BUILDER_TESTWEBREQUESTTRACERBUILDER_H
#define _TEST_CORE_OBJECTS_BUILDER_TESTWEBREQUESTTRACERBUILDER_H

#include "../mock/MockIOpenKitComposite.h"
#include "../../../api/mock/MockILogger.h"
#include "../../../protocol/mock/MockIBeacon.h"

#include "OpenKit/ILogger.h"
#include "core/UTF8String.h"
#include "core/objects/IOpenKitComposite.h"
#include "core/objects/WebRequestTracer.h"
#include "protocol/IBeacon.h"

#include <memory>

namespace test
{
	class TestWebRequestTracerBuilder
	{
	public:

		TestWebRequestTracerBuilder()
			: mLogger(nullptr)
			, mParent(nullptr)
			, mBeacon(nullptr)
			, mUrl(core::objects::WebRequestTracer::UNKNOWN_URL)
		{
		}

		TestWebRequestTracerBuilder& with(std::shared_ptr<openkit::ILogger> logger)
		{
			mLogger = logger;
			return *this;
		}

		TestWebRequestTracerBuilder& with(std::shared_ptr<core::objects::IOpenKitComposite> parent)
		{
			mParent = parent;
			return *this;
		}

		TestWebRequestTracerBuilder& with(std::shared_ptr<protocol::IBeacon> beacon)
		{
			mBeacon = beacon;
			return *this;
		}

		TestWebRequestTracerBuilder& withUrl(const core::UTF8String& url)
		{
			mUrl = url;
			return *this;
		}

		std::shared_ptr<core::objects::WebRequestTracer> build()
		{
			auto logger = mLogger != nullptr ? mLogger : MockILogger::createNice();
			auto parent = mParent != nullptr ? mParent : MockIOpenKitComposite::createNice();
			auto beacon = mBeacon != nullptr ? mBeacon : MockIBeacon::createNice();

			return std::make_shared<core::objects::WebRequestTracer>(
				logger,
				parent,
				beacon,
				mUrl
			);
		}

	private:

		std::shared_ptr<openkit::ILogger> mLogger;
		std::shared_ptr<core::objects::IOpenKitComposite> mParent;
		std::shared_ptr<protocol::IBeacon> mBeacon;
		core::UTF8String mUrl;
	};
}

#endif
