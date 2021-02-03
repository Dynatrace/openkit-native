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

#ifndef _OPENKIT_LOG_LEVEL_H
#define _OPENKIT_LOG_LEVEL_H

#include "OpenKit_export.h"

#include <cstdint>

namespace openkit
{
	///
	/// Log level enumeration.
	/// @remarks This level is used to specify the priority of a log event.
	///
	enum class LogLevel : std::int32_t
	{
		LOG_LEVEL_DEBUG = 0,
		LOG_LEVEL_INFO = 10,
		LOG_LEVEL_WARN = 20,
		LOG_LEVEL_ERROR = 30
	};

	///
	/// Get the log level's name.
	/// @remarks The values retuned are right padded to have an equal length.
	/// @param logLevel The log level for which to get its name.
	/// @return Log level's name.
	///
	OPENKIT_EXPORT const char* getLogLevelName(LogLevel logLevel);
}

#endif
