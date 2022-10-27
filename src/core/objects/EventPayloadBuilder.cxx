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

#include "EventPayloadBuilder.h"
#include "EventPayloadAttributes.h"
#include <OpenKit/json/JsonObjectValue.h>
#include <OpenKit/ILogger.h>
#include <OpenKit/json/JsonStringValue.h>
#include <OpenKit/json/JsonArrayValue.h>

using namespace core::objects;

EventPayloadBuilder::EventPayloadBuilder
(
	openkit::json::JsonObjectValue::JsonObjectMapPtr attributes,
	std::shared_ptr<openkit::ILogger> logger
)
	: mLogger(logger)
	, mAttributes(std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>())
{
	if (attributes != nullptr)
	{
		mAttributes->insert(attributes->begin(), attributes->end());
	}
}

EventPayloadBuilder& EventPayloadBuilder::addOverridableAttribute(const char* key, std::shared_ptr<openkit::json::JsonValue> value)
{
	if (value != nullptr)
	{
		if (mAttributes->find(key) == mAttributes->end())
		{
			mAttributes->insert(std::make_pair(key, value));
		}
	}

	return *this;
}

EventPayloadBuilder& EventPayloadBuilder::addNonOverridableAttribute(const char* key, std::shared_ptr<openkit::json::JsonValue> value)
{
	if (value != nullptr)
	{
		if (mAttributes->find(key) != mAttributes->end())
		{
			mLogger->warning("EventPayloadBuilder addNonOverrideableAttribute: %s is reserved for internal values!", key);
		}

		(* mAttributes)[key] = value;
	}

	return *this;
}

EventPayloadBuilder& EventPayloadBuilder::cleanReservedInternalAttributes()
{
	for (auto it = (* mAttributes).begin(); it != (*mAttributes).end();) {
		if ((*it).first.compare("dt") == 0 || ((*it).first.rfind("dt.", 0) == 0
			&& (*it).first.rfind("dt.agent.", 0) == std::string::npos))
		{
			mLogger->warning("EventPayloadBuilder cleanReservedInternalAttributes: %s is reserved for internal values!", (*it).first.c_str());
			it = (*mAttributes).erase(it);
		}
		else 
		{
			it++;
		}
	}

	return *this;
}

std::string EventPayloadBuilder::build()
{
	return openkit::json::JsonObjectValue::fromMap(mAttributes)->toString();
}
