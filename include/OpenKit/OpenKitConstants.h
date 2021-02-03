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

#ifndef _OPENKIT_OPENKITCONSTANTS_H
#define _OPENKIT_OPENKITCONSTANTS_H

#include "OpenKitVersion.h"

namespace openkit
{
	constexpr char WEBREQUEST_TAG_HEADER[] = "X-dynaTrace";
	constexpr char DEFAULT_APPLICATION_VERSION[] = OPENKIT_VERSION_STRING;
	constexpr char DEFAULT_OPERATING_SYSTEM[] = "OpenKit " OPENKIT_VERSION_STRING;
	constexpr char DEFAULT_MANUFACTURER[] = OPENKIT_COMPANY_NAME;
	constexpr char DEFAULT_MODEL_ID[] = "OpenKitDevice";
}

#endif