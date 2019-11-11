/**
 * Copyright 2018-2019 Dynatrace LLC
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

#ifndef _TEST_CORE_OBJECTS_BUILDER_TESTSESSIONBUILDER_H
#define _TEST_CORE_OBJECTS_BUILDER_TESTSESSIONBUILDER_H

#include "../mock/MockIOpenKitComposite.h"
#include "../../mock/MockIBeaconSender.h"
#include "../../../api/mock/MockILogger.h"
#include "../../../protocol/mock/MockIBeacon.h"

#include "OpenKit/ILogger.h"
#include "core/IBeaconSender.h"
#include "core/objects/IOpenKitComposite.h"
#include "core/objects/Session.h"
#include "protocol/IBeacon.h"

namespace test
{
	class TestSessionBuilder
	{
	public:

		TestSessionBuilder()
			: mLogger(nullptr)
			, mParent(nullptr)
			, mBeacon(nullptr)
		{
		}

		TestSessionBuilder& with(std::shared_ptr<openkit::ILogger> logger)
		{
			mLogger = logger;
			return *this;
		}

		TestSessionBuilder& with(std::shared_ptr<core::objects::IOpenKitComposite> parent)
		{
			mParent = parent;
			return *this;
		}

		TestSessionBuilder& with(std::shared_ptr<protocol::IBeacon> beacon)
		{
			mBeacon = beacon;
			return *this;
		}

		std::shared_ptr<core::objects::Session> build()
		{
			auto logger = mLogger != nullptr ? mLogger : MockILogger::createNice();
			auto parent = mParent != nullptr ? mParent : MockIOpenKitComposite::createNice();
			auto beacon = mBeacon != nullptr ? mBeacon : MockIBeacon::createNice();

			return std::make_shared<core::objects::Session>(
				logger,
				parent,
				beacon
			);
		}

	private:

		std::shared_ptr<openkit::ILogger> mLogger;
		std::shared_ptr<core::objects::IOpenKitComposite> mParent;
		std::shared_ptr<protocol::IBeacon> mBeacon;
	};
}

#endif
