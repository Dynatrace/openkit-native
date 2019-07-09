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

#ifndef _UTIL_JSON_OBJECTS_JSONSTRINGVALUE_H
#define _UTIL_JSON_OBJECTS_JSONSTRINGVALUE_H

#include "util/json/objects/JsonValue.h"
#include "util/json/objects/JsonValueType.h"

#include <memory>
#include <string>

namespace util
{
	namespace json
	{
		namespace objects
		{
			///
			/// JSON value class representing a string value.
			///
			class JsonStringValue : public JsonValue
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
}

#endif //OPENKIT_JSONSTRINGVALUE_H
