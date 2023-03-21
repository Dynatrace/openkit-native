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

#ifndef _CORE_OBJECTS_EVENTPAYLOADBUILDER_H
#define _CORE_OBJECTS_EVENTPAYLOADBUILDER_H

#include <OpenKit/json/JsonValue.h>
#include <OpenKit/json/JsonObjectValue.h>
#include <OpenKit/ILogger.h>
#include <OpenKit/json/JsonArrayValue.h>

namespace core
{
	namespace objects
	{
		class EventPayloadBuilder
		{
		public: 
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="attributes">Dictionary containing attributes for sendEvent API</param>
			/// <param name="logger">Logger for tracing log messages</param>
			EventPayloadBuilder(openkit::json::JsonObjectValue::JsonObjectMapPtr attributes,
				std::shared_ptr<openkit::ILogger> logger);

			/// <summary>
			/// Add an attribute which is overridable
			/// </summary>
			/// <param name="key">Key of the attribute</param>
			/// <param name="value">Value of the attribute</param>
			/// <returns>Builder itself</returns>
			EventPayloadBuilder& addOverridableAttribute(const char* key, std::shared_ptr<openkit::json::JsonValue> value);

			/// <summary>
			/// Add an attribute which is not overridable
			/// </summary>
			/// <param name="key">Key of the attribute</param>
			/// <param name="value">Value of the attribute</param>
			/// <returns>Builder itself</returns>
			EventPayloadBuilder& addNonOverridableAttribute(const char* key, std::shared_ptr<openkit::json::JsonValue> value);

			/// <summary>
			/// Removes reservered internal attributes from the provided attributes
			/// </summary>
			/// <returns>Builder itself</returns>
			EventPayloadBuilder& cleanReservedInternalAttributes();

			/// <summary>
			/// Returns the currently used attributes
			/// </summary>
			/// <returns>Attributes of the builder</returns>
			openkit::json::JsonObjectValue::JsonObjectMapPtr getAttributes() const;

			/// <summary>
			/// Building the whole payload string
			/// </summary>
			/// <returns>Payload string</returns>
			std::string build();

		private:
			/// logger instance
			std::shared_ptr<openkit::ILogger> mLogger;

			/// Internal attributes
			openkit::json::JsonObjectValue::JsonObjectMapPtr mAttributes;

		};
	}
}
#endif