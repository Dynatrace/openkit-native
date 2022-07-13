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

#ifndef _OPENKIT_JSON_JSONARRAYVALUE_H
#define _OPENKIT_JSON_JSONARRAYVALUE_H

#include "OpenKit/OpenKitExports.h"

#include "JsonValue.h"
#include "JsonValueType.h"

#include <list>
#include <cstdint>
#include <memory>

namespace openkit
{
	namespace json
	{
		///
		/// JSON value class representing an array value.
		/// @par {
		///    A JSON array is a composite object that stores other @ref JsonValue "JSON values"
		/// }
		///
		class OPENKIT_EXPORT JsonArrayValue : public JsonValue
		{
		public: // type defs

			using JsonValueList = std::list<std::shared_ptr<JsonValue>>;
			using JsonValueListPtr = std::shared_ptr<JsonValueList>;

		public: // functions

			///
			/// Creates a new JSON array from the given list.
			/// @param[in] values the list of JSON values the created array should hold
			/// @return a newly created @ref JsonArrayValue
			///
			static std::shared_ptr<JsonArrayValue> fromList(const JsonValueListPtr values);

			///
			/// Indicates that this value is of type array.
			///
			JsonValueType getValueType() const override;

			///
			/// Returns the size of this JSON array
			///
			size_t size() const;

			///
			/// Returns an iterator to the first element in this array.
			///
			JsonValueList::iterator begin();

			///
			/// Returns an iterator to the first element in this array.
			///
			JsonValueList::const_iterator begin() const;

			///
			/// Returns an iterator to the last element in this array.
			///
			JsonValueList::iterator end();

			///
			/// Returns an iterator to the last element in the array.
			///
			JsonValueList::const_iterator end() const;

			///
			/// Internal only - Writes the JSON string representation of the JSON value
			/// @param jsonWriter Writer which is able to write the json string
			///
			void writeJsonString(JsonWriter& jsonWriter) const override;

		private: // members

			///
			/// the actual holder of the array elements
			///
			JsonValueListPtr mJsonValues;

		private: // functions

			///
			/// Constructor for initializing this JSON array with a list of values
			/// @param[in] values the underlying values of this JSON array
			///
			JsonArrayValue(const JsonValueListPtr values);
		};
	}
}

#endif //_OPENKIT_JSON_JSONARRAYVALUE_H
