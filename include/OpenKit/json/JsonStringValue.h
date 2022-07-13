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

#ifndef _OPENKIT_JSON_JSONSTRINGVALUE_H
#define _OPENKIT_JSON_JSONSTRINGVALUE_H

#include "OpenKit/OpenKitExports.h"

#include "JsonValue.h"
#include "JsonValueType.h"

#include <memory>
#include <string>

namespace openkit
{
	namespace json
	{
		///
		/// JSON value class representing a string value.
		///
		class OPENKIT_EXPORT JsonStringValue : public JsonValue
		{
		public:

			///
			/// Factory function to create a @ref JsonStringValue and initialize it with the given string
			/// @param stringValue the string value used for initializing this instance
			/// @return a newly created @ref JsonStringValue
			///
			static std::shared_ptr<JsonStringValue> fromString(const std::string& stringValue);

			///
			/// Indicates that this value is of type string
			///
			JsonValueType getValueType() const override;

			///
			/// Returns the underlying string
			///
			const std::string& getValue() const;

			///
			/// Internal only - Writes the JSON string representation of the JSON value
			/// @param jsonWriter Writer which is able to write the json string
			///
			void writeJsonString(JsonWriter& jsonWriter) const override;

		private: // members

			///
			/// the underlying string value
			///
			const std::string mStringValue;

		private: // functions

			///
			/// Constructor taking the underlying string value
			///
			/// @par
			/// to create an instance of @ref JsonStringValue use the factory function @ref ::fromString
			///
			/// @param stringValue the string value of this JSON string.
			///
			JsonStringValue(const std::string& stringValue);
		};
	}
}

#endif //_OPENKIT_JSON_JSONSTRINGVALUE_H
