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

#ifndef _CORE_OBJECTS_SESSIONCREATOR_H
#define _CORE_OBJECTS_SESSIONCREATOR_H

#include "OpenKit/ILogger.h"

#include "ISessionCreator.h"
#include "ISessionCreatorInput.h"

#include "core/UTF8String.h"
#include "core/configuration/IOpenKitConfiguration.h"
#include "core/configuration/IPrivacyConfiguration.h"
#include "protocol/IBeaconInitializer.h"
#include "providers/IThreadIDProvider.h"
#include "providers/ITimingProvider.h"
#include "providers/ISessionIDProvider.h"
#include "providers/IPRNGenerator.h"
#include "core/caching/IBeaconCache.h"

#include <memory>
#include <cstdint>

namespace core
{
	namespace objects
	{
		class SessionCreator 
			: public ISessionCreator
			, public protocol::IBeaconInitializer
		{
		public:

			~SessionCreator() override = default;

			SessionCreator(ISessionCreatorInput& sessionCreatorInput, const char* clientIpAddress);

			std::shared_ptr<SessionInternals> createSession(std::shared_ptr<IOpenKitComposite> parent) override;

			void reset() override;

			std::shared_ptr<openkit::ILogger> getLogger() const override;

			std::shared_ptr<core::caching::IBeaconCache> getBeaconCache() const override;

			bool useClientIpAddress() const override;

			const core::UTF8String& getClientIpAddress() const override;

			std::shared_ptr<providers::ISessionIDProvider> getSessionIdProvider() const override;

			int getSessionSequenceNumber() const override;

			std::shared_ptr<providers::IThreadIDProvider> getThreadIdProvider() const override;

			std::shared_ptr<providers::ITimingProvider> getTiminigProvider() const override;

			std::shared_ptr<providers::IPRNGenerator> getRandomNumberGenerator() const override;

		private:

			/// log message reporter
			const std::shared_ptr<openkit::ILogger> mLogger;

			/// OpenKit related configuration
			const std::shared_ptr<core::configuration::IOpenKitConfiguration> mOpenKitConfiguration;

			/// private related configuration
			const std::shared_ptr<core::configuration::IPrivacyConfiguration> mPrivacyConfiguration;

			// provider for continuously retrieving new session IDs
			const std::shared_ptr<providers::ISessionIDProvider> mContinuousSessionIdProvider;
			
			// provider for continuously retrieving new random numbers.
			const std::shared_ptr<providers::IPRNGenerator> mContinuousRandomNumberGenerator;

			const std::shared_ptr<providers::IThreadIDProvider> mThreadIdProvider;
			const std::shared_ptr<providers::ITimingProvider> mTimingProvider;

			const std::shared_ptr<core::caching::IBeaconCache> mBeaconCache;

			const bool mUseClientIpAddress;
			const core::UTF8String mClientIpAddress;
			const int32_t mServerId;

			std::shared_ptr<providers::ISessionIDProvider> mSessionIdProvider;
			std::shared_ptr<providers::IPRNGenerator> mRandomNumberGenerator;

			int32_t mSessionSequenceNumber;
		};
	}
}

#endif
