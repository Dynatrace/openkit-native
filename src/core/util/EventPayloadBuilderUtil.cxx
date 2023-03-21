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

#include "EventPayloadBuilderUtil.h"
#include <OpenKit/json/JsonNumberValue.h>

using namespace core::util;

bool EventPayloadBuilderUtil::isEventContainingNonFiniteNumericValues(std::shared_ptr<core::objects::EventPayloadBuilder> builder)
{
	auto attributes = builder->getAttributes();
	return EventPayloadBuilderUtil::isObjectContainingNonFiniteNumericValues(openkit::json::JsonObjectValue::fromMap(attributes));
}

bool EventPayloadBuilderUtil::isObjectContainingNonFiniteNumericValues(std::shared_ptr<openkit::json::JsonObjectValue> jsonObject)
{
	for (auto it = jsonObject->begin(); it != jsonObject->end();)
	{
		if (EventPayloadBuilderUtil::isItemContainingNonFiniteNumericValues(it->second))
		{
			return true;
		}

		it++;
	}

	return false;
}

bool EventPayloadBuilderUtil::isArrayContainingNonFiniteNumericValues(std::shared_ptr<openkit::json::JsonArrayValue> jsonArrayValue)
{
	for (auto& value : *jsonArrayValue)
	{
		if (EventPayloadBuilderUtil::isItemContainingNonFiniteNumericValues(value))
		{
			return true;
		}
	}

	return false;
}

bool EventPayloadBuilderUtil::isItemContainingNonFiniteNumericValues(std::shared_ptr<openkit::json::JsonValue> jsonValue)
{
	if (jsonValue->getValueType() == openkit::json::JsonValueType::OBJECT_VALUE
		&& EventPayloadBuilderUtil::isObjectContainingNonFiniteNumericValues(std::dynamic_pointer_cast<openkit::json::JsonObjectValue>(jsonValue)))
	{
		return true;
	}
	else if (jsonValue->getValueType() == openkit::json::JsonValueType::ARRAY_VALUE
		&& EventPayloadBuilderUtil::isArrayContainingNonFiniteNumericValues(std::dynamic_pointer_cast<openkit::json::JsonArrayValue>(jsonValue)))
	{
		return true;
	}
	else if (jsonValue->getValueType() == openkit::json::JsonValueType::NUMBER_VALUE
		&& !std::dynamic_pointer_cast<openkit::json::JsonNumberValue>(jsonValue)->isFinite())
	{
		return true;
	}

	return false;
}