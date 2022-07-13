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

#ifndef _OPENKIT_JSON_JSONVALUE_H
#define _OPENKIT_JSON_JSONVALUE_H

#include "OpenKit/OpenKitExports.h"

#include "JsonValueType.h"
#include <string>

namespace openkit
{
	namespace json
	{
		/// 
		/// Declaration of JsonWriter which is only used internally
		/// 
		class JsonWriter;

		///
		/// Represents the abstract base class for all JSON value classes (e.g. string, number, ...)
		///
		class OPENKIT_EXPORT JsonValue
		{
		public:

			///
			/// Destructor
			///
			virtual ~JsonValue() {}

			///
			/// Returns the @ref JsonValueType "type" of this JSON value
			/// @return the type of this JSON value
			///
			virtual JsonValueType getValueType() const = 0;

			///
			/// Internal only - Writes the JSON string representation of the JSON value
			/// @param jsonWriter Writer which is able to write the json string
			///
			virtual void writeJsonString(JsonWriter& jsonWriter) const = 0;

			///
			/// Returns the JSON string representation of the JSON value
			/// @return the string of this JSON value
			///
			std::string toString() const;
		};
	}
}

#endif //_OPENKIT_JSON_JSONVALUE_H
