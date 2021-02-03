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

#ifndef _UTIL_JSON_READER_IRESETTABLEREADER_H
#define _UTIL_JSON_READER_IRESETTABLEREADER_H

#include <cstdint>

namespace util
{
	namespace json
	{
		namespace reader
		{
			class IResettableReader
			{
			public: // functions

				///
				/// Destructor
				///
				virtual ~IResettableReader() = default;

				///
				/// Reads the next character
				///
				virtual int32_t read() = 0;

				///
				/// Marks the current position to which the reader can be reset.
				///
				/// @param readAheadLimit the number of characters which can be read after marking for a reset operation
				///   to be successful
				///
				virtual void mark(int32_t readAheadLimit) = 0;

				///
				/// Reset the reader to the previously marked position
				///
				virtual void reset() = 0;
			};
		}
	}
}

#endif //_UTIL_JSON_READER_IRESETTABLEREADER_H
