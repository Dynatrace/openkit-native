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

#include "Types.h"

#include <gtest/gtest.h>

using namespace test::types;


class DefaultSessionIDProviderTest : public testing::Test
{
protected:

	void SetUp()
	{
	}

	void TearDown()
	{
	}

	DefaultSessionIdProvider_t* getProvider()
	{
		return &mProvider;
	}

	DefaultSessionIdProvider_t mProvider;
};

TEST_F(DefaultSessionIDProviderTest, defaultSessionIDProviderInitializedWithTimestampReturnsANonNegativeInteger)
{
	//given
	DefaultSessionIdProvider_t* provider = getProvider();

	//when
	int32_t actual = provider->getNextSessionID();

	// then
	ASSERT_GT(actual, 0);
}

TEST_F(DefaultSessionIDProviderTest, defaultSessionIDProviderProvidesConsecutiveNumbers)
{
	//given
	DefaultSessionIdProvider_t* provider = getProvider();

	// when
	int32_t firstSessionID = provider->getNextSessionID();
	int32_t secondSessionID = provider->getNextSessionID();

	// then
	ASSERT_EQ(secondSessionID, firstSessionID + 1);
}

TEST_F(DefaultSessionIDProviderTest, aProviderInitializedWithMaxIntValueProvidesMinSessionIdValueAtNextCall)
{
	//given
	DefaultSessionIdProvider_t provider(INT32_MAX);

	//when
	int32_t actual = provider.getNextSessionID();

	// then
	ASSERT_EQ(actual, 1);
}

TEST_F(DefaultSessionIDProviderTest, aProviderInitializedWithZeroProvidesMinSessionIdValueAtNextCall)
{
	//given
	DefaultSessionIdProvider_t provider(0);

	//when
	int32_t actual = provider.getNextSessionID();

	// then
	ASSERT_EQ(actual, 1);
}