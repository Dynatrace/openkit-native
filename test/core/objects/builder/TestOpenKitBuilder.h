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

#ifndef _CORE_OBJECTS_BUILDER_TESTOPENKITBUILDER_H
#define _CORE_OBJECTS_BUILDER_TESTOPENKITBUILDER_H

#include "../../mock/MockIBeaconSender.h"
#include "../../caching/mock/MockIBeaconCache.h"
#include "../../caching/mock/MockIBeaconCacheEvictor.h"
#include "../../configuration/mock/MockIOpenKitConfiguration.h"
#include "../../configuration/mock/MockIPrivacyConfiguration.h"
#include "../../../api/mock/MockILogger.h"
#include "../../../providers/mock/MockISessionIDProvider.h"
#include "../../../providers/mock/MockITimingProvider.h"
#include "../../../providers/mock/MockIThreadIDProvider.h"

#include "OpenKit/ILogger.h"
#include "core/IBeaconSender.h"
#include "core/caching/IBeaconCache.h"
#include "core/caching/IBeaconCacheEvictor.h"
#include "core/configuration/IOpenKitConfiguration.h"
#include "core/configuration/IPrivacyConfiguration.h"
#include "core/objects/OpenKit.h"
#include "providers/ISessionIDProvider.h"
#include "providers/ITimingProvider.h"
#include "providers/IThreadIDProvider.h"

#include <memory>

namespace test
{
	class TestOpenKitBuilder
	{
	public:

		TestOpenKitBuilder()
			: mLogger(nullptr)
			, mPrivacyConfig(nullptr)
			, mOpenKitConfig(nullptr)
			, mTimingProvider(nullptr)
			, mThreadIdProvider(nullptr)
			, mSessionIdProvider(nullptr)
			, mBeaconCache(nullptr)
			, mBeaconSender(nullptr)
			, mBeaconCacheEvictor(nullptr)
		{
		}

		~TestOpenKitBuilder() = default;

		TestOpenKitBuilder& with(std::shared_ptr<openkit::ILogger> logger)
		{
			mLogger = logger;
			return *this;
		}

		TestOpenKitBuilder& with(std::shared_ptr<core::configuration::IPrivacyConfiguration> privacyConfig)
		{
			mPrivacyConfig = privacyConfig;
			return *this;
		}

		TestOpenKitBuilder& with(std::shared_ptr<core::configuration::IOpenKitConfiguration> openKitConfig)
		{
			mOpenKitConfig = openKitConfig;
			return *this;
		}

		TestOpenKitBuilder& with(std::shared_ptr<providers::ITimingProvider> timingProvider)
		{
			mTimingProvider = timingProvider;
			return *this;
		}

		TestOpenKitBuilder& with(std::shared_ptr<providers::IThreadIDProvider> threadIdProvider)
		{
			mThreadIdProvider = threadIdProvider;
			return *this;
		}

		TestOpenKitBuilder& with(std::shared_ptr<providers::ISessionIDProvider> sessionIdProvider)
		{
			mSessionIdProvider = sessionIdProvider;
			return *this;
		}

		TestOpenKitBuilder& with(std::shared_ptr<core::caching::IBeaconCache> beaconCache)
		{
			mBeaconCache = beaconCache;
			return *this;
		}

		TestOpenKitBuilder& with(std::shared_ptr<core::IBeaconSender> beaconSender)
		{
			mBeaconSender = beaconSender;
			return *this;
		}

		TestOpenKitBuilder& with(std::shared_ptr<core::caching::IBeaconCacheEvictor> beaconCacheEvictor)
		{
			mBeaconCacheEvictor = beaconCacheEvictor;
			return *this;
		}

		std::shared_ptr<core::objects::OpenKit> build()
		{
			auto logger = mLogger != nullptr
				? mLogger
				: MockILogger::createNice();
			auto privacyConfig = mPrivacyConfig != nullptr
				? mPrivacyConfig
				: MockIPrivacyConfiguration::createNice();
			auto openKitConfig = mOpenKitConfig != nullptr
				? mOpenKitConfig
				: MockIOpenKitConfiguration::createNice();
			auto timingProvider = mTimingProvider != nullptr
				? mTimingProvider
				: MockITimingProvider::createNice();
			auto threadIdProvider = mThreadIdProvider != nullptr
				? mThreadIdProvider
				: MockIThreadIDProvider::createNice();
			auto sessionIdProvider = mSessionIdProvider != nullptr
				? mSessionIdProvider
				: MockISessionIDProvider::createNice();
			auto beaconCache = mBeaconCache != nullptr
				? mBeaconCache
				: MockIBeaconCache::createNice();
			auto beaconSender = mBeaconSender != nullptr
				? mBeaconSender
				: MockIBeaconSender::createNice();
			auto beaconCacheEvictor = mBeaconCacheEvictor != nullptr
				? mBeaconCacheEvictor
				: MockIBeaconCacheEvictor::createNice();

			return std::make_shared<core::objects::OpenKit>(
				logger,
				privacyConfig,
				openKitConfig,
				timingProvider,
				threadIdProvider,
				sessionIdProvider,
				beaconCache,
				beaconSender,
				beaconCacheEvictor
			);
		}

	private:

		std::shared_ptr<openkit::ILogger> mLogger;
		std::shared_ptr<core::configuration::IPrivacyConfiguration> mPrivacyConfig;
		std::shared_ptr<core::configuration::IOpenKitConfiguration> mOpenKitConfig;
		std::shared_ptr<providers::ITimingProvider> mTimingProvider;
		std::shared_ptr<providers::IThreadIDProvider> mThreadIdProvider;
		std::shared_ptr<providers::ISessionIDProvider> mSessionIdProvider;
		std::shared_ptr<core::caching::IBeaconCache> mBeaconCache;
		std::shared_ptr<core::IBeaconSender> mBeaconSender;
		std::shared_ptr<core::caching::IBeaconCacheEvictor> mBeaconCacheEvictor;
	};
}

#endif