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

#ifndef OPENKIT_JSONVALUE_H
#define OPENKIT_JSONVALUE_H

#include "util/json/objects/JsonValueType.h"

namespace util
{
	namespace json
	{
		namespace objects
		{
			///
			/// Represents the abstract base class for all JSON value classes (e.g. string, number, ...)
			///
			class JsonValue
			{
			public:

				///
				/// Destructor
				///
				virtual ~JsonValue() {}

				///
				/// Returns the @ref JsonValueType "type" of this JSON value
				/// @return the type of this JSON value
				///
				virtual JsonValueType getValueType() const = 0;
			};
		}
	}
}

#endif //OPENKIT_JSONVALUE_H
