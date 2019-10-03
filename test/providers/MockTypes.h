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

#ifndef _PROVIDERS_MOCKTYPES_H
#define _PROVIDERS_MOCKTYPES_H

#include "MockHTTPClientProvider.h"
#include "MockPRNGenerator.h"
#include "MockSessionIDProvider.h"
#include "MockTimingProvider.h"

namespace test
{
	namespace types
	{
		using MockHttpClientProvider_t = MockHTTPClientProvider;
		using MockNiceHttpClientProvider_t = testing::NiceMock<MockHttpClientProvider_t>;
		using MockNiceHttpClientProvider_sp = std::shared_ptr<MockNiceHttpClientProvider_t>;
		using MockStrictHttpClientProvider_t = testing::StrictMock<MockHttpClientProvider_t>;
		using MockStrictHttpClientProvider_sp = std::shared_ptr<MockStrictHttpClientProvider_t>;

		using MockPrnGenerator_t = MockPRNGenerator;
		using MockNicePrnGenerator_t = testing::NiceMock<MockPrnGenerator_t>;
		using MockNicePrnGenerator_sp = std::shared_ptr<MockNicePrnGenerator_t>;

		using MockSessionIdProvider_t = MockSessionIDProvider;
		using MockNiceSessionIdProvider_t = testing::NiceMock<MockSessionIdProvider_t>;
		using MockNiceSessionIdProvider_sp = std::shared_ptr<MockNiceSessionIdProvider_t>;

		using MockTimingProvider_t = MockTimingProvider;
		using MockNiceTimingProvider_t = testing::NiceMock<MockTimingProvider_t>;
		using MockNiceTimingProvider_sp = std::shared_ptr<MockNiceTimingProvider_t>;
		using MockStrictTimingProvider_t = testing::StrictMock<MockTimingProvider_t>;
		using MockStrictTimingProvider_sp = std::shared_ptr<MockStrictTimingProvider_t>;
	}
}

#endif
