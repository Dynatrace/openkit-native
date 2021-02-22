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

#ifndef _TEST_API_MOCK_MOCKILOGGER_H
#define _TEST_API_MOCK_MOCKILOGGER_H

#include "OpenKit/ILogger.h"

#include "gmock/gmock.h"

#include <stdarg.h>
#include <sstream>
#include <memory>

namespace test
{
	class MockILogger : public openkit::ILogger
	{
	public:

		MockILogger()
		{
			ON_CALL(*this, isErrorEnabled())
				.WillByDefault(testing::Return(true));
			ON_CALL(*this, isWarningEnabled())
				.WillByDefault(testing::Return(true));
			ON_CALL(*this, isDebugEnabled())
				.WillByDefault(testing::Return(true));
			ON_CALL(*this, isInfoEnabled())
				.WillByDefault(testing::Return(true));
		}

		~MockILogger() override = default;

		static std::shared_ptr<testing::NiceMock<MockILogger>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockILogger>>();
		}

		static std::shared_ptr<testing::StrictMock<MockILogger>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockILogger>>();
		}

		void log(openkit::LogLevel, const char* format, ...) override
		{
			va_list args;
			va_start(args, format);
			auto formatted = VarArgsFormatter(&args).formatWith(format);
			mockLog(formatted);
		}

		MOCK_METHOD(void, mockLog, (std::string));


		void error(const char* format, ...) override
		{
			va_list args;
			va_start(args, format);
			auto formatted = VarArgsFormatter(&args).formatWith(format);
			mockError(formatted);
		}

		MOCK_METHOD(void, mockError, (std::string));


		void warning(const char* format, ...) override
		{
			va_list args;
			va_start(args, format);
			auto formatted = VarArgsFormatter(&args).formatWith(format);
			mockWarning(formatted);
		}

		MOCK_METHOD(void, mockWarning, (std::string));

		void info(const char* format, ...) override
		{
			va_list args;
			va_start(args, format);
			auto formatted = VarArgsFormatter(&args).formatWith(format);
			mockInfo(formatted);
		}

		MOCK_METHOD(void, mockInfo, (std::string));


		void debug(const char* format, ...) override
		{
			va_list args;
			va_start(args, format);
			auto formatted = VarArgsFormatter(&args).formatWith(format);
			mockDebug(formatted);
		};

		MOCK_METHOD(void, mockDebug, (std::string));

		MOCK_METHOD(bool, isErrorEnabled, (), (const, override));

		MOCK_METHOD(bool, isInfoEnabled, (), (const, override));

		MOCK_METHOD(bool, isWarningEnabled, (), (const, override));

		MOCK_METHOD(bool, isDebugEnabled, (), (const, override));

	private:

		class VarArgsFormatter
		{
		public:
			VarArgsFormatter(va_list* args)
				: mArgs(args)
			{
			}

			std::string formatWith(const char* format)
			{
				const size_t bufferSize = 4096;
				char buffer[bufferSize];
				if (vsnprintf(buffer, bufferSize, format, *mArgs) < 0)
				{
					throw std::logic_error("cannot format string");
				}

				return std::string(buffer);
			}

			~VarArgsFormatter()
			{
				va_end(*mArgs);
			}

		private:
			va_list* mArgs;
		};
	};
}

#endif
