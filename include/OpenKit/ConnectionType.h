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

#ifndef _OPENKIT_CONNECTIONTYPE_H
#define _OPENKIT_CONNECTIONTYPE_H

#include "OpenKit/OpenKitExports.h"

#include <cstdint>

namespace openkit
{
	///
	/// Specifies the type of a network connection.
	///
	enum class OPENKIT_EXPORT ConnectionType : int32_t
	{
		MOBILE, // Mobile connection type
		WIFI, // Wireless connection type
		OFFLINE, // Offline
		LAN, // Connection via local area network
		UNSET // Reset Connection Type
	};
}

#endif