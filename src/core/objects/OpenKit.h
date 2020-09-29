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

#ifndef _CORE_OBJECTS_OPENKIT_H
#define _CORE_OBJECTS_OPENKIT_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/OpenKitConstants.h"
#include "OpenKit/IOpenKit.h"
#include "OpenKit/IOpenKitBuilder.h"
#include "OpenKit/ILogger.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/ISessionCreatorInput.h"
#include "core/objects/OpenKitComposite.h"
#include "providers/ISessionIDProvider.h"
#include "providers/ITimingProvider.h"
#include "providers/IThreadIDProvider.h"
#include "core/caching/IBeaconCache.h"
#include "core/caching/IBeaconCacheEvictor.h"
#include "core/IBeaconSender.h"

#include <atomic>
#include <mutex>

namespace core
{
	namespace objects
	{
		///
		/// Implementation of IOpenKit interface
		///
		class OpenKit
			: public openkit::IOpenKit
			, public core::objects::OpenKitComposite
			, public core::objects::ISessionCreatorInput
			, public std::enable_shared_from_this<OpenKit>
		{
		public:

			///
			/// Constructor using the default providers for the last the arguments of the specialized constructor
			/// @param[in] logger logging context
			/// @param[in] configuration Configuration object used to build the OpenKit
			///
			OpenKit(
				openkit::IOpenKitBuilder& builder
			);

			///
			/// Constructor not using defaults at all
			/// @param[in] logger logging context
			/// @param[in] privacyConfig configuration holding privacy related details.
			/// @param[in] openKitConfig configuration holding application related details.
			/// @param[in] timingProvider timing provider
			/// @param[in] threadIDProvider provider for thread ids
			/// @param[in] sessionIDProvider provider for new session IDs
			/// @param[in] beaconCache cache where beacon data is stored.
			/// @param[in] beaconSender responsible for sending beacon related data.
			/// @param[in] beaconCacheEvictor evictor to prevent memory over-consumption due to full cache
			///
			OpenKit(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<core::configuration::IPrivacyConfiguration> privacyConfig,
				std::shared_ptr<core::configuration::IOpenKitConfiguration> openKitConfig,
				std::shared_ptr<providers::ITimingProvider> timingProvider,
				std::shared_ptr<providers::IThreadIDProvider> threadIDProvider,
				std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider,
				std::shared_ptr<core::caching::IBeaconCache> beaconCache,
				std::shared_ptr<core::IBeaconSender> beaconSender,
				std::shared_ptr<core::caching::IBeaconCacheEvictor> beaconCacheEvictor
			);

			~OpenKit() override;

			///
			/// Initialize this OpenKit instance.
			/// This method starts the @ref IBeaconSender and is called directly after
			/// the instance has been created in @ref openkit::AbstractOpenKitBuilder.
			///
			void initialize();

			bool waitForInitCompletion() override;

			bool waitForInitCompletion(int64_t timeoutMillis) override;

			bool isInitialized() const override;

			std::shared_ptr<openkit::ISession> createSession(const char* clientIPAddress) override;

			std::shared_ptr<openkit::ISession> createSession() override;

			void shutdown() override;

			void onChildClosed(std::shared_ptr<core::objects::IOpenKitObject> childObject) override;

			void close() override;

			std::shared_ptr<openkit::ILogger> getLogger() override;

			std::shared_ptr<core::configuration::IOpenKitConfiguration> getOpenKitConfiguration() override;

			std::shared_ptr<core::configuration::IPrivacyConfiguration> getPrivacyConfiguration() override;

			std::shared_ptr<core::caching::IBeaconCache> getBeaconCache() override;

			std::shared_ptr<providers::ISessionIDProvider> getSessionIdProvider() override;

			std::shared_ptr<providers::IThreadIDProvider> getThreadIdProvider() override;

			std::shared_ptr<providers::ITimingProvider> getTimingProvider() override;

			int32_t getCurrentServerId() override;

		private:

			///
			/// Method called by the the constructor to perform global initialization.
			/// @remarks This method does check if global init is necessary.
			///
			void globalInit();

			///
			/// Method called by the destructor to perform global destruction.
			/// @remarks This method does check if global destruction is necessary.
			///
			void globalShutdown();

			///
			/// Helper function to write a message upon instance creation.
			///
			/// @param logger the logger to which to write to
			/// @param openKitConfiguration OpneKit related configuration
			///
			static void logOpenKitInstanceCreation(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<core::configuration::IOpenKitConfiguration> openKitConfiguration
			);

		private:

			/// logging context
			const std::shared_ptr<openkit::ILogger> mLogger;

			/// configuration object storing privacy related details.
			const std::shared_ptr<core::configuration::IPrivacyConfiguration> mPrivacyConfiguration;

			/// configuration object storing application related details.
			const std::shared_ptr<core::configuration::IOpenKitConfiguration> mOpenKitConfiguration;

			/// timing provider
			const std::shared_ptr<providers::ITimingProvider> mTimingProvider;

			/// thread id provider
			const std::shared_ptr<providers::IThreadIDProvider> mThreadIDProvider;

			/// session ID provider
			const std::shared_ptr<providers::ISessionIDProvider> mSessionIDProvider;

			/// the beacon cache
			const std::shared_ptr<caching::IBeaconCache> mBeaconCache;

			/// Beacon sender
			const std::shared_ptr<core::IBeaconSender> mBeaconSender;

			/// beacon cache evictor
			const std::shared_ptr<caching::IBeaconCacheEvictor> mBeaconCacheEvictor;

			std::mutex mMutex;

			/// atomic flag for shutdown state
			std::atomic<bool> mIsShutdown;

			/// global instance count
			static int32_t gInstanceCount;

			/// mutex used for global init & de-init
			static std::mutex gInitLock;

		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
