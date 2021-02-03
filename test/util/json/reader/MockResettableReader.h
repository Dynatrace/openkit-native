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

#include "util/json/reader/IResettableReader.h"

#include <gmock/gmock.h>

namespace test
{
	class MockResettableReader : public util::json::reader::IResettableReader
	{
	public:
		MOCK_METHOD0(read, int32_t());

		MOCK_METHOD1(mark, void(int32_t lookAheadLimit));

		MOCK_METHOD0(reset, void());
	};
}