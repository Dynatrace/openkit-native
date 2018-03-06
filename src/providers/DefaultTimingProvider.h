/**
* Copyright 2018 Dynatrace LLC
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

#ifndef _PROVIDERS_DEFAULTTIMINGPROVIDER_H
#define _PROVIDERS_DEFAULTTIMINGPROVIDER_H

#include "ITimingProvider.h"

namespace providers
{
	
	///
	/// Default implementation for timing provider
	///
	class DefaultTimingProvider : public ITimingProvider
	{
	public:

		///
		/// Default constructor
		///
		DefaultTimingProvider();

		///
		/// Provide the current timestamp in milliseconds.
		/// @returns the current timestamp
		///
		virtual int64_t provideTimestampInMilliseconds() override;

		///
		/// Sleep given amount of milliseconds.
		/// @param[in] milliseconds amount of milliseconds to sleep
		///
		virtual void sleep(int64_t milliseconds) override;

		///
		/// Initialize timing provider with cluster time offset. If @c false is provided
		/// for @c isTimeSyncSupported, the cluster offset is set to 0.
		/// @param[in] clusterTimeOffset offset received from cluster
		/// @param[in] isTimeSyncSupported flag if time sync is supported
		///
		virtual void initialize(int64_t clusterTimeOffset, bool isTimeSyncSupported) override;

		///
		/// Returns whether time sync is supported or not
		/// @returns @c true if time sync is supported otherwise @c false
		///
		virtual bool isTimeSyncSupported() override;

		///
		/// Converts a local timestamp to cluster time.
		/// @param timestamp Timestamp in local time
		/// @returns local time if not time synced or if not yet initialized
		///
		virtual int64_t convertToClusterTime(int64_t timestamp) override;

	private:
		/// offset between system-local and cluster time
		int64_t mClusterTimeOffset;

		/// flag if time sync is supported
		bool mIsTimeSyncSupported;

	};
}

#endif