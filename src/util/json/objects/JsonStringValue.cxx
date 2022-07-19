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

#include "OpenKit/json/JsonStringValue.h"
#include "OpenKit/json/JsonValueType.h"
#include "util/json/JsonWriter.h"

using namespace openkit::json;

JsonStringValue::JsonStringValue(const std::string& stringValue) : mStringValue(stringValue)
{
}

std::shared_ptr<JsonStringValue> JsonStringValue::fromString(const std::string& stringValue)
{
	return std::shared_ptr<JsonStringValue>(new JsonStringValue(stringValue));
}

JsonValueType JsonStringValue::getValueType() const
{
	return JsonValueType::STRING_VALUE;
}

const std::string& JsonStringValue::getValue() const
{
	return mStringValue;
}

void JsonStringValue::writeJsonString(JsonWriter& jsonWriter) const
{
	jsonWriter.insertStringValue(mStringValue);
}