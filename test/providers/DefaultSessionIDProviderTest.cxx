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

#include "providers/DefaultSessionIDProvider.h"
#include <gtest/gtest.h>

using namespace providers;


class DefaultSessionIDProviderTest : public testing::Test
{
public:

	void SetUp()
	{
	}

	void TearDown()
	{
	}

	DefaultSessionIDProvider* getProvider()
	{
		return &mProvider;
	}

	DefaultSessionIDProvider mProvider;
};

TEST_F(DefaultSessionIDProviderTest, defaultSessionIDProviderInitializedWithTimestampReturnsANonNegativeInteger)
{
	//given
	DefaultSessionIDProvider* provider = getProvider();

	//when
	int32_t actual = provider->getNextSessionID();

	// then
	ASSERT_GT(actual, 0);
}

TEST_F(DefaultSessionIDProviderTest, defaultSessionIDProviderProvidesConsecutiveNumbers)
{
	//given
	DefaultSessionIDProvider* provider = getProvider();

	// when
	int32_t firstSessionID = provider->getNextSessionID();
	int32_t secondSessionID = provider->getNextSessionID();

	// then
	ASSERT_EQ(secondSessionID, firstSessionID + 1);
}

TEST_F(DefaultSessionIDProviderTest, aProviderInitializedWithMaxIntValueProvidesMinSessionIdValueAtNextCall)
{
	//given
	DefaultSessionIDProvider provider(INT32_MAX);

	//when
	int32_t actual = provider.getNextSessionID();

	// then
	ASSERT_EQ(actual, 1);
}

TEST_F(DefaultSessionIDProviderTest, aProviderInitializedWithZeroProvidesMinSessionIdValueAtNextCall)
{
	//given
	DefaultSessionIDProvider provider(0);

	//when
	int32_t actual = provider.getNextSessionID();

	// then
	ASSERT_EQ(actual, 1);
}