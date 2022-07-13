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

#ifndef _OPENKIT_ILOGGER_H
#define _OPENKIT_ILOGGER_H

#include "OpenKit/OpenKitExports.h"
#include "LogLevel.h"

// The GNU C compiler allows attaching characteristics to function declarations to allow the compiler to perform additional error checking.
#ifndef __GNUC__
// For the MSVC compiler we define the __attribute__ empty
 #define __attribute__(x)
#endif

// The MSC compiler has a similar concept via the SAL Annotation "_Printf_format_string_"
#undef FORMAT_STRING
#if _MSC_VER >= 1400
# include <sal.h>
# if _MSC_VER > 1400
#  define FORMAT_STRING(p) _Printf_format_string_ p
# else
#  define FORMAT_STRING(p) __format_string p
# endif
#else
# define FORMAT_STRING(p) p
#endif

namespace openkit
{
	///
	/// This interface provides logging functionality to OpenKit. By subclassing, OpenKit can make use of custom loggers.
	/// The provided logger interface offers 4 different levels of (increasing) priority: DEBUG, INFO, WARN, ERROR.
	///
	/// Note:
	///   The logger uses a printf syntax with a format string and parameters to be passed via variadic arguments. E.g.:
	///      uint32_t i = 5;
	///      logger.debug("Value of i=%u", i);
	///
	///   The following code snippets show major errors in the logger usage:
	///      std::string var("test");
	///      logger.debug("Value of i=%u", var.c_str()); // writing a c string when the logger expects an unsigned int
	///      logger.debug("Value of i=%s", var);         // writing a std string when the logger expects a c string
	///
	///   To reduce the risk of wrong logger usage the above compiler specifics are used.
	///
	class OPENKIT_EXPORT ILogger
	{
	public:

		///
		/// Destructor
		///
		virtual ~ILogger() {}

		///
		/// Log an event with given level.
		/// Without the additional compiler checks, the function declaration is:
		/// void log(LogLevel logLevel, const char* format, ...);
		/// @param[in] logLevel The log level that is used to log this event
		/// @param[in] format the message string in a printf formatted way with parameters to be passed via varargs
		/// @param[in] ... variadic list of additional arguments
		///
		virtual void log(LogLevel logLevel, FORMAT_STRING(const char* format), ...) __attribute__((format(printf, 3, 4))) = 0;

		///
		/// Log with level 'error'.
		/// Without the additional compiler checks, the function declaration is:
		/// void error(const char* format, ...);
		/// @param[in] format the message string in a printf formatted way with parameters to be passed via varargs
		/// @param[in] ... variadic list of additional arguments
		///
		virtual void error(FORMAT_STRING(const char* format), ...) __attribute__((format(printf, 2, 3))) = 0;

		///
		/// Log with level 'warning'.
		/// Without the additional compiler checks, the function declaration is:
		/// void warning(const char* format, ...);
		/// @param[in] format the message string in a printf formatted way with parameters to be passed via varargs
		/// @param[in] ... variadic list of additional arguments
		///
		virtual void warning(FORMAT_STRING(const char* format), ...) __attribute__((format(printf, 2, 3))) = 0;

		///
		/// Log with level 'info'
		/// Without the additional compiler checks, the function declaration is:
		/// void info(const char* format, ...);
		/// @param[in] format the message string in a printf formatted way with parameters to be passed via varargs
		/// @param[in] ... variadic list of additional arguments
		///
		virtual void info(FORMAT_STRING(const char* format), ...) __attribute__((format(printf, 2, 3))) = 0;

		///
		/// Log with level 'debug'.
		/// Without the additional compiler checks, the function declaration is:
		/// void debug(const char* format, ...);
		/// @param[in] format the message string in a printf formatted way with parameters to be passed via varargs
		/// @param[in] ... variadic list of additional arguments
		///
		virtual void debug(FORMAT_STRING(const char* format), ...) __attribute__((format(printf, 2, 3))) = 0;

		///
		/// Returns a flag if 'error' level messages are currently printed
		/// @return @c true if 'error' level messages are currently printed, @c false otherwise
		///
		virtual bool isErrorEnabled() const = 0;

		///
		/// Returns a flag if 'info' level messages are currently printed
		/// @return @c true if 'info' level messages are currently printed, @c false otherwise
		///
		virtual bool isInfoEnabled() const = 0;

		///
		/// Returns a flag if 'warning' level messages are currently printed
		/// @return @c true if 'warning' level messages are currently printed, @c false otherwise
		///
		virtual bool isWarningEnabled() const = 0;

		///
		/// Returns a flag if 'debug' level messages are currently printed
		/// @return @c true if 'debug' level messages are currently printed, @c false otherwise
		///
		virtual bool isDebugEnabled() const = 0;
	};
}

#endif
