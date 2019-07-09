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

#ifndef _UTIL_JSON_OBJECTS_JSONNUMBERVALUE_H
#define _UTIL_JSON_OBJECTS_JSONNUMBERVALUE_H

#include "util/json/objects/JsonValue.h"
#include "util/json/objects/JsonValueType.h"

#include <string>
#include <memory>

namespace util
{
	namespace json
	{
		namespace objects
		{
			///
			/// Json value class representing a number value.
			///
			/// @par
			/// A number can either be a floating point number or an integer number.
			/// To avoid platform specific problems, all integer numbers can be up to signed 64 bits,
			/// and all floating point values are 64 bits.
			///
			class JsonNumberValue : public JsonValue
			{
			public: // functions

				///
				/// Factory method for constructing a @ref JsonNumberValue from a @c int64_t
				/// @param longValue the long value
				/// @return newly created @ref JsonNumberValue
				///
				static std::shared_ptr<JsonNumberValue> fromLong(int64_t longValue);

				///
				/// Factory method for constructing a @ref JsonNumberValue from a @c double
				/// @param doubleValue the double value
				/// @return newly created @ref JsonNumberValue
				///
				static std::shared_ptr<JsonNumberValue> fromDouble(double doubleValue);

				///
				/// Factory method for constructing a @ref JsonNumberValue from a number literal
				/// @param literalValue the number literal which might either be an integer value or a floating point value.
				/// @return @c null if @c literalValue does not represent a number or a newly created @ref JsonNumberValue
				///
				static std::shared_ptr<JsonNumberValue> fromNumberLiteral(const std::string& literalValue);

				///
				/// Indicates that this value is of type number
				///
				JsonValueType getValueType() const override;

				///
				/// Indicates whether this @ref JsonNumberValue represents an integer value or not.
				/// @return @c true if this instance represents an integer value, @c false otherwise.
				///
				bool isInteger() const;

				///
				/// Indicates whether this @ref JsonNumberValue represents a 32-bit integer or not.
				/// @return @c true if this instance's value fits into a 32 bit integer, @c false otherwise.
				///
				bool isInt32Value() const;

				///
				/// Returns a 32-bit integer value
				///
				/// @par
				/// If this instance is representing a double or in64_t value the result is the value casted to an
				/// @c int32_t
				///
				/// @return a 32-bit integer value representation of this instance's value
				///
				int32_t getInt32Value() const;

				///
				/// Returns a 64-bit integer value
				///
				/// @par
				/// If this instance represents a double then the value is the value casted to an int64_t
				///
				/// @return a 64-bit integer value representation of this instance's value
				///
				int64_t getLongValue() const;

				///
				/// Returns a 32-bit floating point value
				/// @return a 32-bit floating point representation of this instance's value
				///
				float getFloatValue() const;

				///
				/// Returns a 64-bit floating point value.
				/// @return a 64-bit floating point representation of this instance's value
				///
				double getDoubleValue() const;

			private: // members

				///
				/// bool flag indicating whether this is an integer value @c true or a floating point value @c false.
				///
				const bool mIsInteger;

				///
				/// the number stored as 64 bit integer value
				///
				const int64_t mLongValue;

				///
				/// the number stored as double value
				///
				const double mDoubleValue;

			private: // functions

				///
				/// Constructor initializing this @ref JsonNumberValue instance with the given long value
				///
				/// @par
				/// Instead of using this constructor use the @ref ::fromLong factory function.
				///
				/// @param longValue the value representing this instance
				///
				JsonNumberValue(int64_t longValue);

				///
				/// Constructor initializing this @ref JsonNumberValue instance with the given double value.
				///
				/// @par
				/// Instead of using this constructor, use the @ref ::fromDouble factory function.
				///
				/// @param doubleValue the value representing this instance.
				///
				JsonNumberValue(double doubleValue);

				///
				/// Parses a long value from the given string literal value and returns the corresponding @ref JsonNumberValue
				/// @param literalValue the long value as string to be parsed
				/// @return a @ref JsonNumberValue representing a 64-bit integer number
				static std::shared_ptr<JsonNumberValue> parseLongValue(const std::string& literalValue);

				///
				/// Parses a double value from the given string literal and returns the corresponding @ref JsonNumberValue
				/// @param literalValue the double value as string to be parsed.
				/// @return a @ref JsonNumberValue representing a 64-bit floating point number
				static std::shared_ptr<JsonNumberValue> parseDoubleValue(const std::string& literalValue);
			};
		}
	}
}

#endif //_UTIL_JSON_OBJECTS_JSONNUMBERVALUE_H
