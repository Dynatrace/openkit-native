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

#ifndef _OPENKIT_JSON_JSONBOOLEANVALUE_H
#define _OPENKIT_JSON_JSONBOOLEANVALUE_H

#include "OpenKit/OpenKitExports.h"

#include "JsonValue.h"
#include "JsonValueType.h"

#include <string>
#include <memory>

namespace openkit
{
	namespace json
	{
		///
		/// JSON value class representing a boolean value
		///
		class OPENKIT_EXPORT JsonBooleanValue : public JsonValue
		{
		public: // members

			///
			/// Gets sole instance representing true
			///
			static const std::shared_ptr<JsonBooleanValue> trueValue();

			///
			/// Gets sole instance representing false
			///
			static const std::shared_ptr<JsonBooleanValue> falseValue();

		public: // functions

			///
			/// Factory function to create a @ref JsonBooleanValue from a given bool value.
			///
			/// @par
			/// the returned value is one of the two singleton instances @ref TRUE or @ref FALSE
			///
			/// @param value the bool value
			/// @return either @ref FALSE or @ref TRUE
			///
			static std::shared_ptr<JsonBooleanValue> fromValue(bool value);

			///
			/// Factory method to create a @ref JsonBooleanValue from a given JSON literal
			/// @param [in] literal the string literal for which to create the value.
			/// @return @ref TRUE if literal is @ref util::json::constants::BOOLEAN_TRUE_LITERAL, @ref FALSE if
			///   literal is @ref ::util::json::constants::BOOLEAN_FALSE_LITERAL and @c null in every other case.
			///
			static std::shared_ptr<JsonBooleanValue> fromLiteral(const std::string& literal);

			///
			/// Indicates that this value is of type boolean
			///
			JsonValueType getValueType() const override;

			///
			/// Returns the value represented by this instance
			///
			bool getValue() const;

			///
			/// Internal only - Writes the JSON string representation of the JSON value
			/// @param jsonWriter Writer which is able to write the json string
			///
			void writeJsonString(JsonWriter& jsonWriter) const override;

		private: // members

			///
			/// the boolean value represented by this instance
			///
			const bool mValue;

		private: // functions

			///
			/// Constructor taking the boolean value which is represented by this instance.
			///
			/// @par
			/// Use the factory functions @ref ::fromValue or @ref ::fromLiteral instead.
			///
			/// @param value the boolean value represented by this instance
			JsonBooleanValue(bool value);
		};
	}
}

#endif //_OPENKIT_JSON_JSONBOOLEANVALUE_H
