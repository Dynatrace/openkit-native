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

#ifndef _API_C_EXTERNALLOGGER_H
#define _API_C_EXTERNALLOGGER_H

#include "OpenKit/ILogger.h"
#include "api-c/OpenKit-c.h"

#include <cstdarg>

namespace apic
{
	/// The OpenKit comes with a default logger printing trace statements to stdout. 
	/// To override the default logger with a user provided logger from the C binding wrapper,
	/// the @c CustomLogger acts as the glue:
	/// On the one hand it implements the ILogger (C++) interface on the other hand it calls the
	/// user provided function pointers to check if to log and to perform the log.
	class CustomLogger : public openkit::ILogger
	{
	public:
		///
		/// Constructor
		/// @param[in] levelEnabledFunc function pointer to a function checking if the respective log level shall be logged
		/// @param[in] logFunc function pointer to the actual log writing function
		///
		CustomLogger(levelEnabledFunc levelEnabledFunc, logFunc logFunc);
		
		///
		/// Destructor
		///
		virtual ~CustomLogger() {}

		virtual void log(openkit::LogLevel logLevel, const char* format, ...) override;

		virtual void error(const char* format, ...) override;

		virtual void warning(const char* format, ...) override;

		virtual void info(const char* format, ...) override;

		virtual void debug(const char* format, ...) override;

		virtual bool isErrorEnabled() const override;

		virtual bool isWarningEnabled() const override;

		virtual bool isInfoEnabled() const override;

		virtual bool isDebugEnabled() const override;

	private:

		///
		/// Translate the C++ log level to C log level
		/// @param[in] logLevel The C++ log level to translate to C log level.
		/// @return Translated log level
		///
		static LOG_LEVEL cppLogLevelToCLogLevel(openkit::LogLevel logLevel);

		///
		/// Does the actual logging to the provided function pointer
		/// param[in] level the log level of the trace statement
		/// param[in] format the format string in a printf style
		/// param[in] args the list of variable arguments to be passed to the format string
		///
		void doLog(LOG_LEVEL level, const char* format, va_list args);

		/// Function pointer of the function checking if the respective log level shall be logged
		levelEnabledFunc mLevelEnabledFunc;

		/// Function pointer of the actual log writing function
		logFunc mLogFunc;

	};
}

#endif
