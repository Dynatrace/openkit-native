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

#ifndef _CORE_OBJECTS_ISESSIONCREATORINPUT_H
#define _CORE_OBJECTS_ISESSIONCREATORINPUT_H

#include "OpenKit/ILogger.h"

#include "core/configuration/IOpenKitConfiguration.h"
#include "core/configuration/IPrivacyConfiguration.h"
#include "core/caching/IBeaconCache.h"
#include "providers/ISessionIDProvider.h"
#include "providers/IThreadIDProvider.h"
#include "providers/ITimingProvider.h"

#include <memory>
#include <cstdint>

namespace core
{
	namespace objects
	{
		class ISessionCreatorInput
		{
		public:

			///
			/// Destructor
			///
			virtual ~ISessionCreatorInput() = default;

			///
			/// Returns the logger to report/trace messages.
			///
			virtual std::shared_ptr<openkit::ILogger> getLogger() = 0;

			///
			/// Returns the application/device related configuration.
			///
			virtual std::shared_ptr<core::configuration::IOpenKitConfiguration> getOpenKitConfiguration() = 0;

			///
			/// Returns the privacy related configuration.
			///
			virtual std::shared_ptr<core::configuration::IPrivacyConfiguration> getPrivacyConfiguration() = 0;

			///
			/// Returns the beacon cache in which new sessions/beacons will be stored until they are sent.
			///
			virtual std::shared_ptr<core::caching::IBeaconCache> getBeaconCache() = 0;

			///
			/// Returns the provider to obtain the next session ID.
			///
			virtual std::shared_ptr<providers::ISessionIDProvider> getSessionIdProvider() = 0;

			///
			/// Returns the provider to obtain the ID of the current thread.
			///
			virtual std::shared_ptr<providers::IThreadIDProvider> getThreadIdProvider() = 0;

			///
			/// Returns the provider to obtain the current timestamp.
			///
			virtual std::shared_ptr<providers::ITimingProvider> getTimingProvider() = 0;

			///
			/// Returns the current server ID.
			///
			virtual int32_t getCurrentServerId() = 0;
		};
	}
}

#endif
