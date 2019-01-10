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

#ifndef _OPENKIT_CRASHREPORTINGLEVEL_H
#define _OPENKIT_CRASHREPORTINGLEVEL_H

#include "OpenKit_export.h"

#include <cstdint>

namespace openkit
{
	///
	/// This enum declares the crash reporting level to use
	///
	enum class OPENKIT_EXPORT CrashReportingLevel : int32_t
	{
		OFF, // no crash reporting at all
		OPT_OUT_CRASHES, // opt out crash reporting
		OPT_IN_CRASHES // opt in crash reporting
	};
}

#endif
