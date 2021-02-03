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

#ifndef _CORE_CACHING_BEACONKEY_H
#define _CORE_CACHING_BEACONKEY_H

#include <cstddef>
#include <cstdint>

namespace core
{
	namespace caching
	{
		class BeaconKey
		{
		public:

			///
			/// Constructor
			///
			BeaconKey(int32_t beaconId, int32_t beaconSequenceNumber);

			///
			/// Returns the beacon / session number of this key
			///
			int32_t getBeaconId() const;

			///
			/// Returns the beacon / session sequence number of this key
			///
			int32_t getBeaconSequenceNumber() const;

			///
			/// Equality operator.
			///
			bool operator == (const BeaconKey& rhs) const;

			///
			/// Inequality operator.
			///
			bool operator != (const BeaconKey& rhs) const;

			///
			/// Struct for calculating hash code of BeaconKey
			///
			struct Hash
			{
				size_t operator()(const BeaconKey& beaconKey) const noexcept;
			};

		private:

			///
			/// beacon / session number of this key
			///
			int32_t mBeaconId;

			///
			/// beacon / session sequence number of this key
			///
			int32_t mBeaconSequenceNumber;
		};
	}
}

#endif
