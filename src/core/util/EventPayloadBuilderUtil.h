/**
 * Copyright 2018-2023 Dynatrace LLC
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

#ifndef _UTIL_EVENTPAYLOADBUILDERUTIL_H
#define _UTIL_EVENTPAYLOADBUILDERUTIL_H

#include <OpenKit/json/JsonObjectValue.h>
#include <OpenKit/json/JsonArrayValue.h>
#include <core/objects/EventPayloadBuilder.h>

namespace core
{
	namespace util
	{
		///
		/// Utility class for event payload operations
		///
		class EventPayloadBuilderUtil
		{
		public:
			static bool isEventContainingNonFiniteNumericValues(std::shared_ptr<core::objects::EventPayloadBuilder> builder);

		private:
			static bool isObjectContainingNonFiniteNumericValues(std::shared_ptr<openkit::json::JsonObjectValue> jsonObject);
			static bool isArrayContainingNonFiniteNumericValues(std::shared_ptr<openkit::json::JsonArrayValue> jsonArrayValue);
			static bool isItemContainingNonFiniteNumericValues(std::shared_ptr<openkit::json::JsonValue> jsonValue);

			/// utility class, not instantiable
			EventPayloadBuilderUtil() = delete;
		};
	}
}

#endif