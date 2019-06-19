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

#ifndef _CACHING_BEACONCACHERECORD_H
#define _CACHING_BEACONCACHERECORD_H

#include "core/UTF8String.h"

#include <cstdint>

namespace caching
{
	///
	/// Class for a record in the @ref caching::BeaconCache
	///
	class BeaconCacheRecord
	{
	public:
		///
		/// Create a new BeaconCacheRecord.
		/// @param[in] timestamp Timestamp for this record.
		/// @param[in] data      Data to store for this record.
		///
		BeaconCacheRecord(int64_t timestamp, const core::UTF8String& data);

		///
		/// Get timestamp.
		///
		int64_t getTimestamp() const;

		///
		/// Get data.
		///
		const core::UTF8String& getData() const;

		///
		/// Get data size estimation of this record.
		///
		/// Note that this is just a very rough estimation required for cache eviction.
		/// It's sufficient to approximate the bytes required by the string and omit any other information like the timestamp, any references and so on.
		///
		/// @return Data size in bytes.
		///
		int64_t getDataSizeInBytes() const;

		///
		/// Test if this record is already marked for sending.
		/// @return @c true if this record was previously marked for sending, @c false otherwise.
		///
		bool isMarkedForSending() const;

		///
		/// Mark this record for sending (@ref isMarkedForSending()).
		///
		void markForSending();

		///
		/// Reset marked for sending flag (@ref isMarkedForSending()).
		///
		void unsetSending();

	private:
		/// The data's timestamp
		int64_t mTimestamp;

		/// The data of the event or action
		const core::UTF8String mData;

		/// Indicates if this record is marked for sending
		bool mMarkedForSending;
	};

}

#endif
