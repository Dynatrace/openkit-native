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

#ifndef _UTIL_JSON_OBJECTS_JSONOBJECTVALUE_H
#define _UTIL_JSON_OBJECTS_JSONOBJECTVALUE_H

#include "util/json/objects/JsonValue.h"
#include "util/json/objects/JsonValueType.h"

#include <string>
#include <memory>
#include <unordered_map>

namespace util
{
	namespace json
	{
		namespace objects
		{
			///
			/// JSON value class representing an object value.
			///
			class JsonObjectValue : public JsonValue
			{
			public: // type defs

				using JsonObjectMap = std::unordered_map<std::string, std::shared_ptr<JsonValue>>;
				using JsonObjectMapPtr = std::shared_ptr<JsonObjectMap>;

			public: // functions

				///
				/// Factory method for creating a @ref JsonObjectValue
				/// @param jsonObjectMap the map storing the keys and values of the JSON object.
				/// @return the created @ref JsonObjectValue.
				///
				static std::shared_ptr<JsonObjectValue> fromMap(const JsonObjectMapPtr jsonObjectMap);

				///
				/// Indicates that this value is of type object
				///
				JsonValueType getValueType() const override;

				///
				/// Returns the size of the underlying values held by this JSON object.
				/// @return the size of the JSON object
				///
				size_t size() const;

				///
				/// Returns @c true if and only if the given key is present in this JSON object.
				/// @param key the key to test whether it is present or not in this JSON object.
				/// @return @c true if the key is present in this JSON object, @c false otherwise
				///
				bool containsKey(const std::string& key) const;

				///
				/// Returns the value to which the specified key is mapped or @c NULL if this map contains no mapping
				/// for the key.
				/// @param key the key whose associated JSON value is to be returned.
				/// @return the JSON value this key is associated wit or @c null if no such key exists.
				///
				JsonObjectMap::const_iterator find(const std::string& key) const;

				///
				/// Returns the @ref JsonValue associated with the given key
				/// @param key the key for which to return the mapped @ref JsonValue
				///
				std::shared_ptr<JsonValue> operator[] (const std::string& key);

				///
				/// Returns the iterator to the first element
				///
				JsonObjectMap::iterator begin();

				///
				/// Returns the iterator to the first element
				///
				JsonObjectMap::const_iterator begin() const;

				///
				/// Returns the iterator to the last element
				///
				JsonObjectMap::iterator end();

				///
				/// Returns the iterator to the last element
				///
				JsonObjectMap::const_iterator end() const;

			private: // members

				///
				/// the underlying map for this JSON object
				///
				JsonObjectMapPtr mJsonObjectMap;

			private: // functions

				///
				/// Constructor
				///
				/// @par
				/// Use factory function @ref ::fromMap for creating an instance
				///
				/// @param jsonObjectMap the underlying map representing this JSON object.
				///
				JsonObjectValue(const JsonObjectMapPtr jsonObjectMap);
			};
		}
	}
}

#endif //_UTIL_JSON_OBJECTS_JSONOBJECTVALUE_H
