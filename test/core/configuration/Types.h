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

#ifndef _TEST_CONFIGURATION_TYPES_H
#define _TEST_CONFIGURATION_TYPES_H

#include "core/configuration/BeaconCacheConfiguration.h"
#include "core/configuration/BeaconConfiguration.h"
#include "core/configuration/Configuration.h"
#include "core/configuration/Device.h"
#include "core/configuration/HTTPClientConfiguration.h"
#include "core/configuration/OpenKitType.h"

namespace test
{
	namespace types
	{
		using BeaconCacheConfiguration_t = core::configuration::BeaconCacheConfiguration;
		using BeaconCacheConfiguration_sp = std::shared_ptr<BeaconCacheConfiguration_t>;

		using BeaconConfiguration_t = core::configuration::BeaconConfiguration;
		using BeaconConfiguration_sp = std::shared_ptr<BeaconConfiguration_t>;

		using Configuration_t = core::configuration::Configuration;
		using Configuration_sp = std::shared_ptr<Configuration_t>;

		using Device_t = core::configuration::Device;
		using Device_sp = std::shared_ptr<Device_t>;

		using HttpClientConfiguration_t = core::configuration::HTTPClientConfiguration;
		using HttpClientConfiguration_sp = std::shared_ptr<HttpClientConfiguration_t>;

		using OpenKitType_t = core::configuration::OpenKitType;
	}
}

#endif
