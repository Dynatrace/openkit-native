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

#ifndef _CORE_OBJECTS_EVENTPAYLOADATTRIBUTES_H
#define _CORE_OBJECTS_EVENTPAYLOADATTRIBUTES_H

namespace core
{
    namespace objects
	{
		constexpr const char* TIMESTAMP = "timestamp";
		constexpr const char* EVENT_KIND = "event.kind";
		constexpr const char* EVENT_PROVIDER = "event.provider";
		constexpr const char* APP_VERSION = "app.version";
		constexpr const char* OS_NAME = "os.name";
		constexpr const char* DEVICE_MANUFACTURER = "device.manufacturer";
		constexpr const char* DEVICE_MODEL_IDENTIFIER = "device.model.identifier";

		constexpr const char* EVENT_KIND_RUM = "RUM_EVENT";
		constexpr const char* EVENT_KIND_BIZ = "BIZ_EVENT";
	}
}
#endif