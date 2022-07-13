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

#ifndef _OPENKIT_JSON_JSONNULLVALUE_H
#define _OPENKIT_JSON_JSONNULLVALUE_H

#include "OpenKit/OpenKitExports.h"

#include "JsonValue.h"
#include "JsonValueType.h"

#include <memory>

namespace openkit
{
	namespace json
	{
		///
		/// JSON value class representing a null value
		///
		class OPENKIT_EXPORT JsonNullValue : public JsonValue
		{
		public: // members

			///
			/// Gets the sole instance of this class.
			///
			static const std::shared_ptr<JsonNullValue> nullValue();

		public:

			///
			/// Indicates that this value is of type null
			///
			JsonValueType getValueType() const override;

			///
			/// Internal only - Writes the JSON string representation of the JSON value
			/// @param jsonWriter Writer which is able to write the json string
			///
			void writeJsonString(JsonWriter& jsonWriter) const override;

		private: // functions

			///
			/// Constructor
			///
			/// @par
			/// to avoid object churn, use @ref NULL which is the only instance of this class.
			///
			///
			JsonNullValue();
		};
	}
}

#endif //_OPENKIT_JSON_JSONNULLVALUE_H
