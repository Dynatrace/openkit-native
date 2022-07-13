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

#include "util/json/constants/JsonLiterals.h"
#include "OpenKit/json/JsonBooleanValue.h"
#include "util/json/JsonWriter.h"

using namespace openkit::json;
using namespace util::json::constants;

const std::shared_ptr<JsonBooleanValue> JsonBooleanValue::trueValue()
{
	static const auto trueValue = std::shared_ptr<JsonBooleanValue>(new JsonBooleanValue(true));

	return trueValue;
}
const std::shared_ptr<JsonBooleanValue> JsonBooleanValue::falseValue()
{
	static const auto falseValue = std::shared_ptr<JsonBooleanValue>(new JsonBooleanValue(false));

	return falseValue;
}

JsonBooleanValue::JsonBooleanValue(bool value)
	: mValue(value)
{
}

std::shared_ptr<JsonBooleanValue> JsonBooleanValue::fromValue(bool value)
{
	return value ? JsonBooleanValue::trueValue() : JsonBooleanValue::falseValue();
}

std::shared_ptr<JsonBooleanValue> JsonBooleanValue::fromLiteral(const std::string& value)
{
	if (value == JsonLiterals::BOOLEAN_TRUE_LITERAL)
	{
		return JsonBooleanValue::trueValue();
	}
	else if (value == JsonLiterals::BOOLEAN_FALSE_LITERAL)
	{
		return JsonBooleanValue::falseValue();
	}

	return NULL;
}

JsonValueType JsonBooleanValue::getValueType() const
{
	return JsonValueType::BOOLEAN_VALUE;
}

bool JsonBooleanValue::getValue() const
{
	return mValue;
}

void JsonBooleanValue::writeJsonString(JsonWriter& jsonWriter) const
{
	if (mValue) {
		jsonWriter.insertValue("true");
	}
	else {
		jsonWriter.insertValue("false");
	}
}

