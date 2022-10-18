/**
 * Copyright 2018-2022 Dynatrace LLC
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

#include "ConnectionTypeUtil.h"

const char* core::util::ConnectionTypeToString(openkit::ConnectionType cT)
{
	switch (cT)
	{
		case openkit::ConnectionType::MOBILE: return "m";
		case openkit::ConnectionType::WIFI: return "w";
		case openkit::ConnectionType::OFFLINE: return "o";
		case openkit::ConnectionType::LAN: return "l";
		case openkit::ConnectionType::UNSET: return "";
		default: return "";
	}
}