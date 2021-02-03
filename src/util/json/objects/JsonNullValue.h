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

#ifndef _UTIL_JSON_OBJECTS_JSONNULLVALUE_H
#define _UTIL_JSON_OBJECTS_JSONNULLVALUE_H

#include "util/json/objects/JsonValue.h"
#include "util/json/objects/JsonValueType.h"

#include <memory>

namespace util
{
	namespace json
	{
		namespace objects
		{
			///
			/// JSON value class representing a null value
			///
			class JsonNullValue : public JsonValue
			{
			public: // members

				///
				/// Gets the sole instance of this class.
				///
				static const std::shared_ptr<JsonNullValue> nullValue();

			public:

				///
				/// Indicates that this value is of type null
				///
				JsonValueType getValueType() const override;

			private: // functions

				///
				/// Constructor
				///
				/// @par
				/// to avoid object churn, use @ref NULL which is the only instance of this class.
				///
				///
				JsonNullValue();
			};
		}
	}
}

#endif //_UTIL_JSON_OBJECTS_JSONNULLVALUE_H
