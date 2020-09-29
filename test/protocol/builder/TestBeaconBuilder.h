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

#ifndef _TEST_PROTOCOL_BUILDER_TESTBEACONBUILDER_H
#define _TEST_PROTOCOL_BUILDER_TESTBEACONBUILDER_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "../../api/mock/MockILogger.h"
#include "../../core/caching/mock/MockIBeaconCache.h"
#include "../../core/configuration/mock/MockIBeaconConfiguration.h"
#include "../../providers/mock/MockIPRNGenerator.h"
#include "../../providers/mock/MockISessionIDProvider.h"
#include "../../providers/mock/MockIThreadIDProvider.h"
#include "../../providers/mock/MockITimingProvider.h"

#include "OpenKit/ILogger.h"
#include "core/UTF8String.h"
#include "core/caching/IBeaconCache.h"
#include "protocol/Beacon.h"
#include "providers/IPRNGenerator.h"
#include "providers/ISessionIDProvider.h"
#include "providers/IThreadIDProvider.h"
#include "providers/ITimingProvider.h"

namespace test
{
	class TestBeaconBuilder
	{
	public:

		TestBeaconBuilder()
			: mLogger(nullptr)
			, mBeaconCache(nullptr)
			, mConfiguration(nullptr)
			, mClientIPAddress("127.0.0.1")
			, mThreadIDProvider(nullptr)
			, mTimingProvider(nullptr)
		{
		}

		virtual ~TestBeaconBuilder() = default;

		TestBeaconBuilder& with(std::shared_ptr<openkit::ILogger> logger)
		{
			mLogger = logger;
			return *this;
		}

		TestBeaconBuilder& with(std::shared_ptr<core::caching::IBeaconCache> beaconCache)
		{
			mBeaconCache = beaconCache;
			return *this;
		}

		TestBeaconBuilder& with(std::shared_ptr<core::configuration::IBeaconConfiguration> configuration)
		{
			mConfiguration = configuration;
			return *this;
		}

		TestBeaconBuilder& withIpAddress(core::UTF8String& ipAddress)
		{
			return withIpAddress(ipAddress.getStringData().c_str());
		}

		TestBeaconBuilder& withIpAddress(const char* ipAddress)
		{
			mClientIPAddress = ipAddress;
			return *this;
		}

		TestBeaconBuilder& with(std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider)
		{
			mSessionIDProvider = sessionIDProvider;
			return *this;
		}

		TestBeaconBuilder& with(std::shared_ptr<providers::IThreadIDProvider> threadIDProvider)
		{
			mThreadIDProvider = threadIDProvider;
			return *this;
		}

		TestBeaconBuilder& with(std::shared_ptr<providers::ITimingProvider> timingProvider)
		{
			mTimingProvider = timingProvider;
			return *this;
		}

		TestBeaconBuilder& with(std::shared_ptr<providers::IPRNGenerator> prnGenerator)
		{
			mPRNGenerator = prnGenerator;
			return *this;
		}

		std::shared_ptr<protocol::Beacon> build()
		{
			auto logger = (mLogger != nullptr)
				? mLogger
				: MockILogger::createNice();
			auto beaconCache = (mBeaconCache != nullptr)
				? mBeaconCache
				: MockIBeaconCache::createNice();
			auto configuration = (mConfiguration != nullptr)
				? mConfiguration
				: MockIBeaconConfiguration::createNice();
			auto sessionIDProvider = (mSessionIDProvider != nullptr)
				? mSessionIDProvider
				: MockISessionIDProvider::createNice();
			auto threadIDProvider = (mThreadIDProvider != nullptr)
				? mThreadIDProvider
				: MockIThreadIDProvider::createNice();
			auto timingProvider = (mTimingProvider != nullptr)
				? mTimingProvider
				: MockITimingProvider::createNice();

			if(mPRNGenerator != nullptr)
			{
				return std::make_shared<protocol::Beacon>(
					logger,
					beaconCache,
					configuration,
					mClientIPAddress,
					sessionIDProvider,
					threadIDProvider,
					timingProvider,
					mPRNGenerator
				);
			}

			return std::make_shared<protocol::Beacon>(
				logger,
				beaconCache,
				configuration,
				mClientIPAddress,
				sessionIDProvider,
				threadIDProvider,
				timingProvider
			);
		}

	private:

		std::shared_ptr<openkit::ILogger> mLogger;
		std::shared_ptr<core::caching::IBeaconCache> mBeaconCache;
		std::shared_ptr<core::configuration::IBeaconConfiguration> mConfiguration;
		const char* mClientIPAddress;
		std::shared_ptr<providers::ISessionIDProvider> mSessionIDProvider;
		std::shared_ptr<providers::IThreadIDProvider> mThreadIDProvider;
		std::shared_ptr<providers::ITimingProvider> mTimingProvider;
		std::shared_ptr<providers::IPRNGenerator> mPRNGenerator;
	};
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
