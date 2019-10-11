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

#ifndef _TEST_API_MOCKLOGGER_H
#define _TEST_API_MOCKLOGGER_H

#include "OpenKit/ILogger.h"
#include "Types.h"
#include "../api/Types.h"

#include "gmock/gmock.h"

#include <stdarg.h>
#include <sstream>

namespace test
{
	class MockILogger : public types::ILogger_t
	{
	public:

		virtual ~MockILogger() {}

		void log(openkit::LogLevel, const char* format, ...) override
		{
			va_list args;
			va_start(args, format);
			auto formatted = VarArgsFormatter(&args).formatWith(format);
			mockLog(formatted);
		}

		MOCK_METHOD1(mockLog, void(std::string));


		void error(const char* format, ...) override
		{
			va_list args;
			va_start(args, format);
			auto formatted = VarArgsFormatter(&args).formatWith(format);
			mockError(formatted);
		}

		MOCK_METHOD1(mockError, void(std::string));


		void warning(const char* format, ...) override
		{
			va_list args;
			va_start(args, format);
			auto formatted = VarArgsFormatter(&args).formatWith(format);
			mockWarning(formatted);
		}

		MOCK_METHOD1(mockWarning, void(std::string));

		void info(const char* format, ...) override
		{
			va_list args;
			va_start(args, format);
			auto formatted = VarArgsFormatter(&args).formatWith(format);
			mockInfo(formatted);
		}

		MOCK_METHOD1(mockInfo, void(std::string));


		void debug(const char* format, ...) override
		{
			va_list args;
			va_start(args, format);
			auto formatted = VarArgsFormatter(&args).formatWith(format);
			mockDebug(formatted);
		};

		MOCK_METHOD1(mockDebug, void(std::string));

		MOCK_CONST_METHOD0(isErrorEnabled, bool());

		MOCK_CONST_METHOD0(isInfoEnabled, bool());

		MOCK_CONST_METHOD0(isWarningEnabled, bool());

		MOCK_CONST_METHOD0(isDebugEnabled, bool());

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
