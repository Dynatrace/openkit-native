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

#ifndef _TEST_CORE_COMMUNICATION_BUILDER_TESTBEACONSENDINGCONTEXTBUILDER_H
#define _TEST_CORE_COMMUNICATION_BUILDER_TESTBEACONSENDINGCONTEXTBUILDER_H

#include "../mock/MockIBeaconSendingState.h"
#include "../../configuration/mock/MockIHTTPClientConfiguration.h"
#include "../../../core/util/mock/MockIInterruptibleThreadSuspender.h"
#include "../../../api/mock/MockILogger.h"
#include "../../../providers/mock/MockIHTTPClientProvider.h"
#include "../../../providers/mock/MockITimingProvider.h"

#include "OpenKit/ILogger.h"
#include "core/communication/BeaconSendingContext.h"
#include "core/communication/IBeaconSendingState.h"
#include "core/configuration/IHTTPClientConfiguration.h"
#include "core/util/IInterruptibleThreadSuspender.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"

#include <memory>

namespace test
{
	class TestBeaconSendingContextBuilder
	{
	public:
		TestBeaconSendingContextBuilder()
			: mLogger(nullptr)
			, mClientConfig(nullptr)
			, mClientProvider(nullptr)
			, mTimingProvider(nullptr)
			, mThreadSuspender(nullptr)
			, mState(nullptr)
		{
		}

		TestBeaconSendingContextBuilder& with(std::shared_ptr<openkit::ILogger> logger)
		{
			mLogger = logger;
			return *this;
		}

		TestBeaconSendingContextBuilder& with(
			std::shared_ptr<core::configuration::IHTTPClientConfiguration> clientConfig
		)
		{
			mClientConfig = clientConfig;
			return *this;
		}

		TestBeaconSendingContextBuilder& with(std::shared_ptr<providers::IHTTPClientProvider> clientProvider)
		{
			mClientProvider = clientProvider;
			return *this;
		}

		TestBeaconSendingContextBuilder& with(std::shared_ptr<providers::ITimingProvider> timingProvider)
		{
			mTimingProvider = timingProvider;
			return *this;
		}

		TestBeaconSendingContextBuilder& with(std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender)
		{
			mThreadSuspender = threadSuspender;
			return *this;
		}

		TestBeaconSendingContextBuilder& with(std::unique_ptr<core::communication::IBeaconSendingState> state)
		{
			mState = std::move(state);
			return *this;
		}

		std::shared_ptr<core::communication::BeaconSendingContext> build()
		{
			auto logger = mLogger != nullptr ? mLogger : MockILogger::createNice();
			auto clientConfig = mClientConfig != nullptr ? mClientConfig : MockIHTTPClientConfiguration::createNice();
			auto clientProvider = mClientProvider != nullptr ? mClientProvider : MockIHTTPClientProvider::createNice();
			auto timingProvider = mTimingProvider != nullptr ? mTimingProvider : MockITimingProvider::createNice();
			auto threadSuspender = mThreadSuspender != nullptr ? mThreadSuspender : MockIInterruptibleThreadSuspender::createNice();

			if (mState != nullptr)
			{
				return std::make_shared<core::communication::BeaconSendingContext>(
					logger,
					clientConfig,
					clientProvider,
					timingProvider,
					threadSuspender,
					std::move(mState)
				);
			}

			return std::make_shared<core::communication::BeaconSendingContext>(
				logger,
				clientConfig,
				clientProvider,
				timingProvider,
				threadSuspender
			);
		}

	private:
		std::shared_ptr<openkit::ILogger> mLogger;
		std::shared_ptr<core::configuration::IHTTPClientConfiguration> mClientConfig;
		std::shared_ptr<providers::IHTTPClientProvider> mClientProvider;
		std::shared_ptr<providers::ITimingProvider> mTimingProvider;
		std::shared_ptr<core::util::IInterruptibleThreadSuspender> mThreadSuspender;
		std::unique_ptr<core::communication::IBeaconSendingState> mState;
	};
}

#endif
