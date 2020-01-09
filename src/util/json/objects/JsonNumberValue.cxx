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
#include "util/json/objects/JsonNumberValue.h"

#include <stdexcept>
#include <sstream>

using namespace util::json::objects;
using namespace util::json::constants;


JsonNumberValue::JsonNumberValue(int64_t longValue)
	: mIsInteger(true)
	, mLongValue(longValue)
	, mDoubleValue(static_cast<double>(longValue))
{
}

JsonNumberValue::JsonNumberValue(double doubleValue)
	: mIsInteger(false)
	, mLongValue(static_cast<int64_t>(doubleValue))
	, mDoubleValue(doubleValue)
{
}

std::shared_ptr<JsonNumberValue> JsonNumberValue::fromLong(int64_t longValue)
{
	return std::shared_ptr<JsonNumberValue>(new JsonNumberValue(longValue));
}

std::shared_ptr<JsonNumberValue> JsonNumberValue::fromDouble(double doubleValue)
{
	return std::shared_ptr<JsonNumberValue>(new JsonNumberValue(doubleValue));
}

std::shared_ptr<JsonNumberValue> JsonNumberValue::fromNumberLiteral(const std::string& literalValue)
{
	std::smatch matches;

	if (!std::regex_match(literalValue, matches, JsonLiterals::getNumberPattern()))
	{
		return nullptr;
	}

	try
	{
		if (!matches[2].matched && !matches[3].matched)
		{
			// only integer part matched
			return parseLongValue(literalValue);
		}
		return parseDoubleValue(literalValue);
	}
	catch (std::invalid_argument&)
	{
		return nullptr;
	}
}

std::shared_ptr<JsonNumberValue> JsonNumberValue::parseLongValue(const std::string& literalValue)
{
	std::istringstream iss(literalValue);

	int64_t parsedValue;
	iss >> parsedValue;

	if (iss.fail() || !iss.eof())
	{
		return nullptr;
	}

	return JsonNumberValue::fromLong(parsedValue);
}

std::shared_ptr<JsonNumberValue> JsonNumberValue::parseDoubleValue(const std::string& literalValue)
{
	std::istringstream iss(literalValue);

	double parsedValue;
	iss >> parsedValue;

	if(iss.fail() || !iss.eof())
	{
		return nullptr;
	}

	return JsonNumberValue::fromDouble(parsedValue);
}

JsonValueType JsonNumberValue::getValueType() const
{
	return JsonValueType::NUMBER_VALUE;
}

bool JsonNumberValue::isInteger() const
{
	return mIsInteger;
}

bool JsonNumberValue::isInt32Value() const
{
	return isInteger() && mLongValue >= INT32_MIN && mLongValue <= INT32_MAX;
}

int32_t JsonNumberValue::getInt32Value() const
{
	return static_cast<int32_t>(mLongValue);
}

int64_t JsonNumberValue::getLongValue() const
{
	return mLongValue;
}

float JsonNumberValue::getFloatValue() const
{
	return static_cast<float>(mDoubleValue);
}

double JsonNumberValue::getDoubleValue() const
{
	return mDoubleValue;
}
