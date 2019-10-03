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

#ifndef _CORE_CACHING_IBEACONCACHEEVICTIONSTRATEGY_H
#define _CORE_CACHING_IBEACONCACHEEVICTIONSTRATEGY_H

namespace core
{
	namespace caching
	{
		///
		/// An implementor of this interface shall evict @ref BeaconCacheEntry based on strategy's rules.
		///
		class IBeaconCacheEvictionStrategy
		{
		public:
			///
			/// Destructor
			///
			virtual ~IBeaconCacheEvictionStrategy() {}

			///
			/// Called when this strategy is executed.
			///
			virtual void execute() = 0;
		};

	}
}

#endif
