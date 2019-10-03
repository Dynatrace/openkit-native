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

#ifndef _TEST_CORE_CACHING_TYPES_H
#define _TEST_CORE_CACHING_TYPES_H

#include "core/caching/BeaconCache.h"
#include "core/caching/BeaconCacheEntry.h"
#include "core/caching/BeaconCacheEvictor.h"
#include "core/caching/BeaconCacheRecord.h"
#include "core/caching/IBeaconCache.h"
#include "core/caching/IBeaconCacheEvictionStrategy.h"
#include "core/caching/IObserver.h"

namespace test
{
	namespace types
	{
		using BeaconCache_t = core::caching::BeaconCache;
		using BeaconCache_sp = std::shared_ptr<BeaconCache_t>;

		using BeaconCacheEntry_t = core::caching::BeaconCacheEntry;
		using BeaconCacheEntry_sp = std::shared_ptr<BeaconCache_t>;

		using BeaconCacheEvictor_t = core::caching::BeaconCacheEvictor;
		using BeaconCacheEvictor_sp = std::shared_ptr<BeaconCache_t>;

		using BeaconCacheRecord_t = core::caching::BeaconCacheRecord;
		using BeaconCacheRecord_sp = std::shared_ptr<BeaconCacheRecord_t>;

		using IBeaconCache_t = core::caching::IBeaconCache;
		using IBeaconCache_sp = std::shared_ptr<IBeaconCache_t>;

		using IBeaconCacheEvictionStrategy_t = core::caching::IBeaconCacheEvictionStrategy;
		using IBeaconCacheEvictionStrategy_sp = std::shared_ptr<IBeaconCacheEvictionStrategy_t>;

		using IObserver_t = core::caching::IObserver;
		using IObserver_sp = std::shared_ptr<IObserver_t>;
	}
}

#endif
