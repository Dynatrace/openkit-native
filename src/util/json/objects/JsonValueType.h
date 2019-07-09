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

#ifndef _OPENKIT_JSONVALUETYPE_H
#define _OPENKIT_JSONVALUETYPE_H

#include "util/json/objects/JsonValue.h"

namespace util
{
	namespace json
	{
		namespace objects
		{
			///
			/// Specifies the type of a respective @ref JsonValue
			///
			enum class JsonValueType
			{
				///
				/// Specifies a JSON null value.
				///
				NULL_VALUE,

				///
				/// Specifies a JSON boolean value.
				///
				BOOLEAN_VALUE,

				///
				/// Specifies a JSON numeric value.
				///
				NUMBER_VALUE,

				///
				/// Specifies a JSON string value.
				///
				STRING_VALUE,

				///
				/// Specifies a JSON array value.
				///
				ARRAY_VALUE,

				///
				/// Specifies a JSON object value.
				///
				OBJECT_VALUE
			};
		}
	}
}

#endif //_OPENKIT_JSONVALUETYPE_H
