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

#include "protocol/http/NullHttpRequestInterceptor.h"
#include "../../api/mock/MockIHttpRequest.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace test;

using NullHttpRequestInterceptor_t = protocol::NullHttpRequestInterceptor;

class NullHttpRequestInterceptorTest : public testing::Test
{
};

TEST_F(NullHttpRequestInterceptorTest, instanceReturnsAlwaysTheSameInstance)
{
	// when
	auto obtained = NullHttpRequestInterceptor_t::instance();

	// then
	ASSERT_THAT(NullHttpRequestInterceptor_t::instance(), testing::Eq(obtained));
}

TEST_F(NullHttpRequestInterceptorTest, interceptDoesNotInteractWithHttpRequest)
{
	// given
	auto httpRequestMock = MockIHttpRequest::createStrict();
	NullHttpRequestInterceptor_t target;

	// when, then
	target.intercept(*httpRequestMock);
}