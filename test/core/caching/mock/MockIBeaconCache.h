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

#ifndef _TEST_CORE_CACHING_MOCK_MOCKIBEACONCACHE_H
#define _TEST_CORE_CACHING_MOCK_MOCKIBEACONCACHE_H

#include "core/UTF8String.h"
#include "core/caching/IBeaconCache.h"
#include "core/caching/IObserver.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIBeaconCache : public core::caching::IBeaconCache
	{
	public:
		MockIBeaconCache() = default;

		~MockIBeaconCache() override = default;

		static std::shared_ptr<testing::NiceMock<MockIBeaconCache>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIBeaconCache>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIBeaconCache>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIBeaconCache>>();
		}

		MOCK_METHOD(void, addObserver, (core::caching::IObserver*), (override));

		MOCK_METHOD(
			void,
			addEventData,
			(
				const core::caching::BeaconKey&,
				int64_t,
				const core::UTF8String&
			),
			(override)
		);

		MOCK_METHOD(
			void,
			addActionData,
			(
				const core::caching::BeaconKey&,
				int64_t,
				const core::UTF8String&
			),
			(override)
		);

		MOCK_METHOD(
			void,
			deleteCacheEntry,
			(
				const core::caching::BeaconKey&
			),
			(override)
		);

		MOCK_METHOD(
			void,
			prepareDataForSending,
			(
				const core::caching::BeaconKey&
			),
			(override)
		);

		MOCK_METHOD(
			bool,
			hasDataForSending,
			(
				const core::caching::BeaconKey&
			),
			(override)
		);

		MOCK_METHOD(
			const core::UTF8String,
			getNextBeaconChunk,
			(
				const core::caching::BeaconKey&,
				const core::UTF8String&,
				int32_t,
				const core::UTF8String&
			),
			(override)
		);

		MOCK_METHOD(
			void,
			removeChunkedData,
			(
				const core::caching::BeaconKey&
			),
			(override)
		);

		MOCK_METHOD(
			void,
			resetChunkedData,
			(
				const core::caching::BeaconKey&
			),
			(override)
		);

		MOCK_METHOD(
			(const std::unordered_set<core::caching::BeaconKey, core::caching::BeaconKey::Hash>),
			getBeaconKeys,
			(),
			(override)
		);

		MOCK_METHOD(
			uint32_t,
			evictRecordsByAge,
			(
				const core::caching::BeaconKey&,
				int64_t
			),
			(override)
		);

		MOCK_METHOD(
			uint32_t,
			evictRecordsByNumber,
			(
				const core::caching::BeaconKey&,
				uint32_t
			),
			(override)
		);

		MOCK_METHOD(int64_t, getNumBytesInCache, (), (const, override));

		MOCK_METHOD(
			bool,
			isEmpty,
			(
				const core::caching::BeaconKey&
			),
			(override)
		);
	};
}
#endif
