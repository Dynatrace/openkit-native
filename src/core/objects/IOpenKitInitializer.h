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

#ifndef _CORE_OBJECTS_IOPENKITINITIALIZER_H
#define _CORE_OBJECTS_IOPENKITINITIALIZER_H

#include "OpenKit/ILogger.h"

#include "core/IBeaconSender.h"
#include "core/ISessionWatchdog.h"
#include "core/caching/IBeaconCache.h"
#include "core/caching/IBeaconCacheEvictor.h"
#include "core/configuration/IOpenKitConfiguration.h"
#include "core/configuration/IPrivacyConfiguration.h"
#include "providers/ISessionIDProvider.h"
#include "providers/IThreadIDProvider.h"
#include "providers/ITimingProvider.h"

#include <memory>

namespace core
{
	namespace objects
	{
		///
		/// Provides relevant data for initializing / creating an OpenKit instance.
		///
		class IOpenKitInitializer
		{
		public:

			///
			/// Destructor
			///
			virtual ~IOpenKitInitializer() = default;

			///
			/// Returns ILogger for reporting messages.
			///
			virtual std::shared_ptr<openkit::ILogger> getLogger() const = 0;

			///
			/// Returns privacy settings determining which kind of data is collected.
			///
			virtual std::shared_ptr<core::configuration::IPrivacyConfiguration> getPrivacyConfiguration() const = 0;

			///
			/// Returns OpenKit / application related configuration.
			///
			virtual std::shared_ptr<core::configuration::IOpenKitConfiguration> getOpenKitConfiguration() const = 0;

			///
			/// Returns provider used to obtain the current timestamp.
			///
			virtual std::shared_ptr<providers::ITimingProvider> getTimingProvider() const = 0;

			///
			/// Returns providers for the identification of the current thread.
			///
			virtual std::shared_ptr<providers::IThreadIDProvider> getThreadIdProvider() const = 0;

			///
			/// Returns provider to obtain the identifier for the next session.
			///
			virtual std::shared_ptr<providers::ISessionIDProvider> getSessionIdProvider() const = 0;

			///
			/// Returns cache where beacon data is stored until it is sent.
			///
			virtual std::shared_ptr<core::caching::IBeaconCache> getBeaconCache() const = 0;

			///
			/// Returns eviction thread to prevent beacon cache from overflowing.
			///
			virtual std::shared_ptr<core::caching::IBeaconCacheEvictor> getBeaconCacheEvictor() const = 0;

			///
			/// Returns sender thread for sending beacons to the server.
			///
			virtual std::shared_ptr<core::IBeaconSender> getBeaconSender() const = 0;

			///
			/// Returns watchdog thread to perform certain actions for sessions at/after a specific time.
			///
			virtual std::shared_ptr<core::ISessionWatchdog> getSessionWatchdog() const = 0;
		};
	}
}

#endif
