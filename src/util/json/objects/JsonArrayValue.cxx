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

#include "util/json/objects/JsonArrayValue.h"

using namespace util::json::objects;


JsonArrayValue::JsonArrayValue(const JsonValueList& values)
		: mJsonValues(values)
{
}

std::shared_ptr<JsonArrayValue> JsonArrayValue::fromList(const JsonValueList& values)
{
	return std::shared_ptr<JsonArrayValue>(new JsonArrayValue(values));
}

JsonValueType JsonArrayValue::getValueType() const
{
	return JsonValueType::ARRAY_VALUE;
}

size_t JsonArrayValue::size() const
{
	return mJsonValues.size();
}

JsonArrayValue::JsonValueList::iterator JsonArrayValue::begin()
{
	return mJsonValues.begin();
}

JsonArrayValue::JsonValueList::const_iterator JsonArrayValue::begin() const
{
	return mJsonValues.begin();
}

JsonArrayValue::JsonValueList::iterator JsonArrayValue::end()
{
	return mJsonValues.end();
}

JsonArrayValue::JsonValueList::const_iterator JsonArrayValue::end() const
{
	return mJsonValues.end();
}