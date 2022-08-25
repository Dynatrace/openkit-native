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

#ifndef _OPENKIT_JSON_JSONWRITER_H
#define _OPENKIT_JSON_JSONWRITER_H

#include <sstream>
#include <string>

namespace openkit
{
	namespace json
	{
		///
	/// JSON writer class for writing a JSON string
	///
		class JsonWriter
		{

		public:

			///
			/// Default constructor
			///
			JsonWriter();

			///
			/// Appending characters for opening an array in a JSON string
			///
			void openArray();

			///
			/// Appending characters for closing an array in a JSON string 
			///
			void closeArray();

			///
			/// Appending characters for opening an object in a JSON string
			///
			void openObject();

			///
			/// Appending characters for closing an object in a JSON string
			///
			void closeObject();

			/// 
			/// Appending characters for a key in a JSON string
			/// 
			/// @param key JSON key
			/// 
			void insertKey(const std::string& key);

			/// 
			/// Appending characters for a string value in a JSON string
			/// 
			/// @param value JSON string value
			/// 
			void insertStringValue(const std::string& value);

			/// 
			/// Appending characters for a value in a JSON string
			/// 
			/// @param value JSON value
			/// 
			void insertValue(const std::string& value);

			/// 
			/// Appending characters for seperating a key value pair in a JSON string
			/// 
			void insertKeyValueSeperator();

			/// 
			/// Appending characters for seperating arrays, objects and values in a JSON string
			/// 
			void insertElementSeperator();

			/// 
			/// Returning the whole JSON string
			/// 
			const std::string toString();

		private: // functions

			///
			/// Escaping characters for JSON output
			/// 
			/// @param value JSON string value which should be escaped
			/// @return Escaped string value
			///
			const std::string escapeString(const std::string& value);

		private: // members

			///
			/// the underlying string stream
			///
			std::stringstream mStringStream;
		};
	}
}

#endif //_OPENKIT_JSON_JSONWRITER_H