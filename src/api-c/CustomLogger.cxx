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

#include "CustomLogger.h"

using namespace apic;

CustomLogger::CustomLogger(levelEnabledFunc levelEnabledFunc, logFunc logFunc)
	: mLevelEnabledFunc(levelEnabledFunc)
	, mLogFunc(logFunc)
{
}

void CustomLogger::error(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	doLog(LOG_LEVEL::LOGLEVEL_ERROR, format, args);
	va_end(args);
}

void CustomLogger::warning(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	doLog(LOG_LEVEL::LOGLEVEL_WARN, format, args);
	va_end(args);
}

void CustomLogger::info(const char *format, ...)
{
	if (isInfoEnabled())
	{
		va_list args;
		va_start(args, format);
		doLog(LOG_LEVEL::LOGLEVEL_INFO, format, args);
		va_end(args);
	}
}

void CustomLogger::debug(const char *format, ...)
{
	if (isDebugEnabled())
	{
		va_list args;
		va_start(args, format);
		doLog(LOG_LEVEL::LOGLEVEL_DEBUG, format, args);
		va_end(args);
	}
}

bool CustomLogger::isErrorEnabled() const
{
	return mLevelEnabledFunc(LOG_LEVEL::LOGLEVEL_ERROR);
}
bool CustomLogger::isWarningEnabled() const
{
	return mLevelEnabledFunc(LOG_LEVEL::LOGLEVEL_WARN);
}

bool CustomLogger::isInfoEnabled() const
{
	return mLevelEnabledFunc(LOG_LEVEL::LOGLEVEL_INFO);
}

bool CustomLogger::isDebugEnabled() const
{
	return mLevelEnabledFunc(LOG_LEVEL::LOGLEVEL_DEBUG);
}

void CustomLogger::doLog(LOG_LEVEL level, const char* format, va_list args)
{
	// perform the printf argument filling
	va_list argcopy;
	va_copy(argcopy, args);
	int length = vsnprintf(nullptr, 0, format, argcopy);	// excl. term. 0
	va_end(argcopy);
	char *traceStatement = new char[length + 1]; // + 1 for term. 0
	vsnprintf(traceStatement, length + 1, format, args);

	// call the provided logging function with the resulting string
	mLogFunc(level, traceStatement);

	// cleanup
	delete[] traceStatement;
}
	
