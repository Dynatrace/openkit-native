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

#ifndef _API_OPENKITCONSTANTS_H
#define _API_OPENKITCONSTANTS_H

namespace api
{
	constexpr char WEBREQUEST_TAG_HEADER[] = "X-dynaTrace";
	//TODO: stefan.eberl: generate the version using cmake
	constexpr char DEFAULT_APPLICATION_VERSION[] = "1.0.1";
	constexpr char DEFAULT_OPERATING_SYSTEM[] = "OpenKit 1.0.1";
	constexpr char DEFAULT_MANUFACTURER[] = "Dynatrace";
	constexpr char DEFAULT_MODEL_ID[] = "OpenKitDevice";
}

#endif