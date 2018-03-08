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

#ifndef _TEST_PROTOCOL_MOCKSTATUSRESPONSE_H
#define _TEST_PROTOCOL_MOCKSTATUSRESPONSE_H

#include "protocol/StatusResponse.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace test
{
	class MockStatusResponse : public protocol::StatusResponse
	{
	public:
		MockStatusResponse()
			: protocol::StatusResponse("", 200)
		{
		}

		virtual ~MockStatusResponse() {};

		MOCK_CONST_METHOD0(getResponseCode, uint32_t());
		MOCK_CONST_METHOD0(isCapture, bool());
	};
}
#endif
