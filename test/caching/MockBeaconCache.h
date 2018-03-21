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

#ifndef _TEST_CACHING_MOCKBEACONCACHE_H
#define _TEST_CACHING_MOCKBEACONCACHE_H

#include "caching/IBeaconCache.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace caching;
namespace test
{
	class MockBeaconCache : public IBeaconCache
	{
	public:
		MockBeaconCache()
		{
		}

		virtual ~MockBeaconCache() {}

		MOCK_METHOD1(addObserver, void(IObserver*));
		MOCK_METHOD3(addEventData, void(int32_t, int64_t, const core::UTF8String&));
		MOCK_METHOD3(addActionData, void(int32_t, int64_t, const core::UTF8String&));
		MOCK_METHOD1(deleteCacheEntry, void(int32_t));
		MOCK_METHOD4(getNextBeaconChunk, const core::UTF8String(int32_t, const core::UTF8String&, int32_t, const char*));
		MOCK_METHOD1(removeChunkedData, void(int32_t));
		MOCK_METHOD1(resetChunkedData, void(int32_t));
		MOCK_METHOD0(getBeaconIDs, const std::unordered_set<int32_t>());
		MOCK_METHOD2(evictRecordsByAge, uint32_t(int32_t, int64_t));
		MOCK_METHOD2(evictRecordsByNumber, uint32_t(int32_t, uint32_t));
		MOCK_CONST_METHOD0(getNumBytesInCache, const int64_t());
		MOCK_METHOD1(isEmpty, bool(int32_t));
	};
}
#endif
