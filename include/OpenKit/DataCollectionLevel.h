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

#ifndef _OPENKIT_DATACOLLECTIONLEVEL_H
#define _OPENKIT_DATACOLLECTIONLEVEL_H

#include "OpenKit_export.h"

#include <cstdint>

namespace openkit
{
	///
	/// This enum declares the data collection level to use
	///
	enum class OPENKIT_EXPORT DataCollectionLevel : int32_t
	{
		OFF, // data collection is off
		PERFORMANCE, // only collect performance data but omit user behavior data
		USER_BEHAVIOR // collect all data
	};
}

#endif
