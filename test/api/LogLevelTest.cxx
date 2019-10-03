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

class LogLevelTest : public ::testing::Test
{
};

TEST_F(LogLevelTest, getLogLevelNameForLogLevelDebug)
{
	// when
	auto obtained = openkit::getLogLevelName(LogLevel_t::LOG_LEVEL_DEBUG);

	// then
	ASSERT_STREQ("DEBUG", obtained);
}

TEST_F(LogLevelTest, getLogLevelNameForLogLevelInfo)
{
	// when
	auto obtained = openkit::getLogLevelName(LogLevel_t::LOG_LEVEL_INFO);

	// then
	ASSERT_STREQ("INFO ", obtained);
}

TEST_F(LogLevelTest, getLogLevelNameForLogLevelWarn)
{
	// when
	auto obtained = openkit::getLogLevelName(LogLevel_t::LOG_LEVEL_WARN);

	// then
	ASSERT_STREQ("WARN ", obtained);
}

TEST_F(LogLevelTest, getLogLevelNameForLogLevelError)
{
	// when
	auto obtained = openkit::getLogLevelName(LogLevel_t::LOG_LEVEL_ERROR);

	// then
	ASSERT_STREQ("ERROR", obtained);
}

TEST_F(LogLevelTest, getLogLevelNameForUnknownLogLevel)
{
	// when
	auto obtained = openkit::getLogLevelName(LogLevel_t(666));

	// then
	ASSERT_STREQ("N/A  ", obtained);
}
