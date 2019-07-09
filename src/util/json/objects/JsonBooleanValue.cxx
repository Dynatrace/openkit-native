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

#include "util/json/constants/JsonLiterals.h"
#include "util/json/objects/JsonBooleanValue.h"

using namespace util::json::objects;
using namespace util::json::constants;


const std::shared_ptr<JsonBooleanValue> JsonBooleanValue::TRUE = std::shared_ptr<JsonBooleanValue>(new JsonBooleanValue(true));
const std::shared_ptr<JsonBooleanValue> JsonBooleanValue::FALSE = std::shared_ptr<JsonBooleanValue>(new JsonBooleanValue(false));

JsonBooleanValue::JsonBooleanValue(bool value)
	: mValue(value)
{
}

std::shared_ptr<JsonBooleanValue> JsonBooleanValue::fromValue(bool value)
{
	return value ? JsonBooleanValue::TRUE : JsonBooleanValue::FALSE;
}

std::shared_ptr<JsonBooleanValue> JsonBooleanValue::fromLiteral(const std::string& value)
{
	if (value == JsonLiterals::BOOLEAN_TRUE_LITERAL)
	{
		return JsonBooleanValue::TRUE;
	}
	else if (value == JsonLiterals::BOOLEAN_FALSE_LITERAL)
	{
		return JsonBooleanValue::FALSE;
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

