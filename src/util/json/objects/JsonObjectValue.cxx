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

#include "util/json/objects/JsonObjectValue.h"

using namespace util::json::objects;


JsonObjectValue::JsonObjectValue(const JsonObjectMapPtr jsonObjectMap)
	: mJsonObjectMap(jsonObjectMap)
{
}

std::shared_ptr<JsonObjectValue> JsonObjectValue::fromMap(const JsonObjectMapPtr jsonObjectMap)
{
	return std::shared_ptr<JsonObjectValue>(new JsonObjectValue(jsonObjectMap));
}

JsonValueType JsonObjectValue::getValueType() const
{
	return JsonValueType::OBJECT_VALUE;
}

size_t JsonObjectValue::size() const
{
	return mJsonObjectMap->size();
}

bool JsonObjectValue::containsKey(const std::string& key) const
{
	auto entry = mJsonObjectMap->find(key);
	return entry != mJsonObjectMap->end();
}

JsonObjectValue::JsonObjectMap::const_iterator JsonObjectValue::find(const std::string& key) const
{
	return mJsonObjectMap->find(key);
}

std::shared_ptr<JsonValue> JsonObjectValue::operator[] (const std::string& key)
{
	return (*mJsonObjectMap)[key];
}

JsonObjectValue::JsonObjectMap::iterator JsonObjectValue::begin()
{
	return mJsonObjectMap->begin();
}

JsonObjectValue::JsonObjectMap::const_iterator JsonObjectValue::begin() const
{
	return mJsonObjectMap->begin();
}

JsonObjectValue::JsonObjectMap::iterator JsonObjectValue::end()
{
	return mJsonObjectMap->end();
}

JsonObjectValue::JsonObjectMap::const_iterator JsonObjectValue::end() const
{
	return mJsonObjectMap->end();
}