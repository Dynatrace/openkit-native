/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _UTIL_DEFAULTLOGGER_H
#define _UTIL_DEFAULTLOGGER_H

#include "OpenKit/ILogger.h"
#include "OpenKit/LogLevel.h"

#include <ostream>

namespace core
{
	namespace util
	{
		///
		/// Default implementation of @ref openkit::ILogger which write to std::cout
		///
		class DefaultLogger : public openkit::ILogger
		{
		public:
			///
			/// Default constructor
			///
			DefaultLogger();

			///
			/// Constructor
			/// param[in] logLevel The log level of this logger
			///
			DefaultLogger(openkit::LogLevel logLevel);

			///
			/// Constructor to let the default logger write to the provided stream. Intended for unit testing.
			/// param[in] stream the stream where the default logger shall write to
			/// param[in] logLevel The log level of this logger
			///
			DefaultLogger(std::ostream &stream, openkit::LogLevel logLevel);

			///
			/// Destructor
			///
			~DefaultLogger() override = default;

			void log(openkit::LogLevel logLevel, const char* format, ...) override;

			void error(const char* format, ...) override;

			void warning(const char* format, ...) override;

			void info(const char* format, ...) override;

			void debug(const char* format, ...) override;

			bool isErrorEnabled() const override;

			bool isWarningEnabled() const override;

			bool isInfoEnabled() const override;

			bool isDebugEnabled() const override;

		private:
			///
			/// Does the actual logging to the @ref mStream.
			/// param[in] level the log level is added to the trace
			/// param[in] format the format string in a printf style
			/// param[in] args the list of variable arguments to be passed to printf
			///
			void doLog(const char* level, const char* format, va_list args);

			/// The stream to be logged to
			std::ostream &mStream;

			/// Log level of this Logger instance.
			/// @remarks Log messages with higher or same priority are logged,
			/// all others are ignored.
			openkit::LogLevel mLogLevel;
		};
	}
}

#endif
