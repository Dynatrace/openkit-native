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

#ifndef _CORE_CONFIGURATION_BEACONCACHECONFIGURATION_H
#define _CORE_CONFIGURATION_BEACONCACHECONFIGURATION_H

#include "IBeaconCacheConfiguration.h"
#include "OpenKit/IOpenKitBuilder.h"

#include <cstdint>

namespace core
{
	namespace configuration
	{
		///
		/// Configuration for beacon cache.
		///
		class BeaconCacheConfiguration
			: public IBeaconCacheConfiguration
		{
		public:
			///
			/// Constructor
			/// @param[in] builder OpenKit builder providing the necessary beacon cache configuration details
			///
			BeaconCacheConfiguration(const openkit::IOpenKitBuilder& builder);

			///
			/// Creates a beacon cache configuration from the given OpenKit builder.
			///
			/// @param builder the OpenKit builder from which to create a beacon cache configuration.
			/// @return a newly created beacon cache configuration instance or @c nullptr if the given argument is
			/// @c nullptr
			///
			static std::shared_ptr<IBeaconCacheConfiguration> from(const openkit::IOpenKitBuilder& builder);

			///
			/// Get maximum record age.
			///
			int64_t getMaxRecordAge() const override;

			///
			/// Get lower memory limit for the cache.
			///
			int64_t getCacheSizeLowerBound() const override;


			///
			/// Get upper memory limit for the cache.
			///
			int64_t getCacheSizeUpperBound() const override;

		private:
			/// maximum record age
			int64_t mMaxRecordAge;

			/// lower memory limit for the cache
			int64_t mCacheSizeLowerBound;

			/// upper memory limit for the cache
			int64_t mCacheSizeUpperBound;
		};
	}
}

#endif