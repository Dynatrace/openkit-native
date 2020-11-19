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

		MOCK_METHOD1(addObserver,
			void(
				core::caching::IObserver*
			)
		);

		MOCK_METHOD3(addEventData,
			void(
				const core::caching::BeaconKey&,
				int64_t,
				const core::UTF8String&
			)
		);

		MOCK_METHOD3(addActionData,
			void(
				const core::caching::BeaconKey&,
				int64_t,
				const core::UTF8String&
			)
		);

		MOCK_METHOD1(deleteCacheEntry,
			void(
				const core::caching::BeaconKey&
			)
		);

		MOCK_METHOD4(getNextBeaconChunk,
			const core::UTF8String(
				const core::caching::BeaconKey&,
				const core::UTF8String&,
				int32_t,
				const core::UTF8String&
			)
		);

		MOCK_METHOD1(removeChunkedData,
			void(
				const core::caching::BeaconKey&
			)
		);

		MOCK_METHOD1(resetChunkedData,
			void(
				const core::caching::BeaconKey&
			)
		);

		MOCK_METHOD0(getBeaconKeys, const std::unordered_set<core::caching::BeaconKey, core::caching::BeaconKey::Hash>());

		MOCK_METHOD2(evictRecordsByAge,
			uint32_t(
				const core::caching::BeaconKey&,
				int64_t
			)
		);

		MOCK_METHOD2(evictRecordsByNumber,
			uint32_t(
				const core::caching::BeaconKey&,
				uint32_t
			)
		);

		MOCK_CONST_METHOD0(getNumBytesInCache, int64_t());

		MOCK_METHOD1(isEmpty,
			bool(
				const core::caching::BeaconKey&
			)
		);
	};
}
#endif
