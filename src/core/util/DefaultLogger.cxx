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

#include "core/util/DefaultLogger.h"

#include <thread>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdarg>

using namespace core::util;

DefaultLogger::DefaultLogger()
	: DefaultLogger(openkit::LogLevel::LOG_LEVEL_WARN)
{
}

DefaultLogger::DefaultLogger(openkit::LogLevel logLevel)
	: DefaultLogger(std::cout, logLevel)
{
}

DefaultLogger::DefaultLogger(std::ostream &stream, openkit::LogLevel logLevel)
	: mStream(stream)
	, mLogLevel(logLevel)
{
}

void DefaultLogger::log(openkit::LogLevel logLevel, const char* format, ...)
{
	if (logLevel < mLogLevel)
	{
		return;
	}

	va_list args;
	va_start(args, format);
	doLog(openkit::getLogLevelName(logLevel), format, args);
	va_end(args);
}

void DefaultLogger::error(const char *format, ...)
{
	if (!isErrorEnabled())
	{
		return;
	}

	va_list args;
	va_start(args, format);
	doLog(openkit::getLogLevelName(openkit::LogLevel::LOG_LEVEL_ERROR), format, args);
	va_end(args);
}

void DefaultLogger::warning(const char *format, ...)
{
	if (!isWarningEnabled())
	{
		return;
	}

	va_list args;
	va_start(args, format);
	doLog(openkit::getLogLevelName(openkit::LogLevel::LOG_LEVEL_WARN), format, args);
	va_end(args);
}

void DefaultLogger::info(const char *format, ...)
{
	if (!isInfoEnabled())
	{
		return;
	}

	va_list args;
	va_start(args, format);
	doLog(openkit::getLogLevelName(openkit::LogLevel::LOG_LEVEL_INFO), format, args);
	va_end(args);
}

void DefaultLogger::debug(const char *format, ...)
{
	if (!isDebugEnabled())
	{
		return;
	}

	va_list args;
	va_start(args, format);
	doLog(openkit::getLogLevelName(openkit::LogLevel::LOG_LEVEL_DEBUG), format, args);
	va_end(args);
}

bool DefaultLogger::isErrorEnabled() const
{
	return openkit::LogLevel::LOG_LEVEL_ERROR >= mLogLevel;
}
bool DefaultLogger::isWarningEnabled() const
{
	return openkit::LogLevel::LOG_LEVEL_WARN >= mLogLevel;
}

bool DefaultLogger::isInfoEnabled() const
{
	return openkit::LogLevel::LOG_LEVEL_INFO >= mLogLevel;
}

bool DefaultLogger::isDebugEnabled() const
{
	return openkit::LogLevel::LOG_LEVEL_DEBUG >= mLogLevel;
}

void DefaultLogger::doLog(const char * level, const char* format, va_list args)
{
	std::stringstream msg;

	// add current date/time in format "YYYY-MM-DD HH:mm:ss"
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	struct tm tmNow;
#if defined (_MSC_VER)
	localtime_s(&tmNow, &in_time_t);
#else
	localtime_r(&in_time_t, &tmNow);
#endif
	msg << std::put_time(&tmNow, "%Y-%m-%d %X") << " " ;
	
	// add the log level
	msg << level << " [";

	// add thread id
	msg << std::this_thread::get_id() << "] ";

	// add the trace statement
	va_list argcopy;
	va_copy(argcopy, args);
	size_t length = vsnprintf(nullptr, 0, format, argcopy);	// excl. term. 0
	va_end(argcopy);
	char *traceStatement = new char[length + 1]; // + 1 for term. 0
	vsnprintf(traceStatement, length + 1, format, args);
	msg << traceStatement;
	delete[] traceStatement;

	// finally print out the whole trace
	mStream << msg.str() << std::endl;
}
