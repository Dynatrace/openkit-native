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


#ifndef _CORE_OBJECTS_OPENKIT_H
#define _CORE_OBJECTS_OPENKIT_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/OpenKitConstants.h"
#include "OpenKit/IOpenKit.h"
#include "OpenKit/ILogger.h"
#include "core/configuration/Configuration.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/OpenKitComposite.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"
#include "providers/IThreadIDProvider.h"
#include "core/caching/IBeaconCache.h"
#include "core/caching/IBeaconCacheEvictor.h"
#include "core/IBeaconSender.h"
#include "NullSession.h"

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
			, public std::enable_shared_from_this<OpenKit>
		{
		public:

			///
			/// Constructor using the default providers for the last the arguments of the specialized constructor
			/// @param[in] logger logging context
			/// @param[in] configuration Configuration object used to build the OpenKit
			///
			OpenKit(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<configuration::Configuration> configuration
			);

			///
			/// Constructor not using defaults at all
			/// @param[in] logger logging context
			/// @param[in] configuration Configuration object used to build the OpenKit
			/// @param[in] httpClientProvider provider for HTTP clients
			/// @param[in] timingProvider timing provider
			/// @param[in] threadIDProvider provider for thread ids
			///
			OpenKit(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<configuration::Configuration> configuration,
				std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
				std::shared_ptr<providers::ITimingProvider> timingProvider,
				std::shared_ptr<providers::IThreadIDProvider> threadIDProvider
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

			void shutdown() override;

			void onChildClosed(std::shared_ptr<core::objects::IOpenKitObject> childObject) override;

			void close() override;

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

		private:

			/// logging context
			std::shared_ptr<openkit::ILogger> mLogger;

			/// HTTP client provider
			std::shared_ptr<providers::IHTTPClientProvider> mClientProvider;

			/// the configuration used the build the OpenKit instance
			std::shared_ptr<configuration::Configuration> mConfiguration;

			/// timing provider
			std::shared_ptr<providers::ITimingProvider> mTimingProvider;

			/// thread id provider
			std::shared_ptr<providers::IThreadIDProvider> mThreadIDProvider;

			/// the beacon cache
			std::shared_ptr<caching::IBeaconCache> mBeaconCache;

			/// Beacon sender
			std::shared_ptr<core::IBeaconSender> mBeaconSender;

			/// beacon cache evictor
			std::shared_ptr<caching::IBeaconCacheEvictor> mBeaconCacheEvictor;

			/// atomic flag for shutdown state
			std::atomic<int32_t> mIsShutdown;

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