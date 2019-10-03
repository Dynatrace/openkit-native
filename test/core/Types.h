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

#ifndef _TEST_CORE_TYPES_H
#define _TEST_CORE_TYPES_H

#include "core/BeaconSender.h"
#include "core/SessionWrapper.h"
#include "core/UTF8String.h"

namespace test
{
	namespace types
	{
		using BeaconSender_t = core::BeaconSender;
		using BeaconSender_sp = std::shared_ptr<BeaconSender_t>;

		using SessionWrapper_t = core::SessionWrapper;
		using SessionWrapper_sp = std::shared_ptr<SessionWrapper_t>;

		using Utf8String_t = core::UTF8String;
		using Utf8String_sp = std::shared_ptr<Utf8String_t>;
	}
}

#endif
