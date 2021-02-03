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

#ifndef _CORE_OBJECTS_OPENKITINITIALIZER_H
#define _CORE_OBJECTS_OPENKITINITIALIZER_H

#include "OpenKit/IOpenKitBuilder.h"

#include "core/objects/IOpenKitInitializer.h"

namespace core
{
	namespace objects
	{
		///
		/// Provides relevant data for initializing / creating an OpenKit instance.
		///
		class OpenKitInitializer
			: public IOpenKitInitializer
		{
		public:

			OpenKitInitializer(const openkit::IOpenKitBuilder& builder);

			~OpenKitInitializer() override = default;

			std::shared_ptr<openkit::ILogger> getLogger() const override;

			std::shared_ptr<core::configuration::IPrivacyConfiguration> getPrivacyConfiguration() const override;

			std::shared_ptr<core::configuration::IOpenKitConfiguration> getOpenKitConfiguration() const override;

			std::shared_ptr<providers::ITimingProvider> getTimingProvider() const override;

			std::shared_ptr<providers::IThreadIDProvider> getThreadIdProvider() const override;

			std::shared_ptr<providers::ISessionIDProvider> getSessionIdProvider() const override;

			std::shared_ptr<core::caching::IBeaconCache> getBeaconCache() const override;

			std::shared_ptr<core::caching::IBeaconCacheEvictor> getBeaconCacheEvictor() const override;

			std::shared_ptr<core::IBeaconSender> getBeaconSender() const override;

			std::shared_ptr<core::ISessionWatchdog> getSessionWatchdog() const override;

		private:

			std::shared_ptr<openkit::ILogger> mLogger;
			std::shared_ptr<core::configuration::IPrivacyConfiguration> mPrivacyConfiguration;
			std::shared_ptr<core::configuration::IOpenKitConfiguration> mOpenKitConfiguration;
			std::shared_ptr<providers::ITimingProvider> mTimingProvider;
			std::shared_ptr<providers::IThreadIDProvider> mThreadIdProvider;
			std::shared_ptr<providers::ISessionIDProvider> mSessionIdProvider;
			std::shared_ptr<core::caching::IBeaconCache> mBeaconCache;
			std::shared_ptr<core::caching::IBeaconCacheEvictor> mBeaconCacheEvictor;
			std::shared_ptr<core::IBeaconSender> mBeaconSender;
			std::shared_ptr<core::ISessionWatchdog> mSessionWatchdog;
		};
	}
}

#endif
