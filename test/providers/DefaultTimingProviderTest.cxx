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

#include "providers/DefaultTimingProvider.h"
#include <gtest/gtest.h>
#include <chrono>

using namespace providers;

/**
* DefaultTimingProvider that always returns the same value for provideTimestampInMilliseconds
*/
class TestDefaultTimingProvider : public DefaultTimingProvider
{
public:
	TestDefaultTimingProvider(int64_t now)
		: timestampNow(now)
	{
	}

	virtual int64_t provideTimestampInMilliseconds() override
	{
		return timestampNow;
	}

private:
	int64_t timestampNow;
};

class DefaultTimingProviderTest : public testing::Test
{
public:
	DefaultTimingProviderTest()
		: mClusterOffset(0)
		, mNow(0)
		, mProvider(mNow)
	{
	}

	void SetUp()
	{
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
			);

		mNow = ms.count();
		mClusterOffset = 1234LL;
	}

	void TearDown()
	{
	}

	TestDefaultTimingProvider* getProvider()
	{
		return &mProvider;
	}

	int64_t getClusterOffset()
	{
		return mClusterOffset;
	}

	int64_t getCurrentTimestamp()
	{
		return mNow;
	}

private:

	int64_t mClusterOffset;
	int64_t mNow;

	TestDefaultTimingProvider mProvider;
};

TEST_F(DefaultTimingProviderTest, TimeSyncIsSupportedByDefault)
{
	// given
	auto provider = getProvider();

	// then
	EXPECT_TRUE(provider->isTimeSyncSupported());
}


TEST_F(DefaultTimingProviderTest, TimeSyncIsSupportedIfInitCalledWithTrue)
{
	//given
	auto provider = getProvider();

	// when
	provider->initialize(0L, true);

	// then
	EXPECT_TRUE(provider->isTimeSyncSupported());
}

TEST_F(DefaultTimingProviderTest, TimeSyncIsNotSupportedIfInitCalledWithFalse)
{
	//given
	auto provider = getProvider();

	// when
	provider->initialize(0L, false);

	// then
	EXPECT_FALSE(provider->isTimeSyncSupported());
}

TEST_F(DefaultTimingProviderTest, ATimingProviderCanConvertToClusterTime)
{
	//given
	auto provider = getProvider();
	provider->initialize(getClusterOffset(), true);

	// when
	int64_t target = provider->convertToClusterTime(getCurrentTimestamp());

	// then
	EXPECT_EQ(target, getClusterOffset() + getCurrentTimestamp());
}
