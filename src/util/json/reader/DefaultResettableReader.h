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

#ifndef _UTIL_JSON_READER_DEFAULTRESETTABLEREADER_H
#define _UTIL_JSON_READER_DEFAULTRESETTABLEREADER_H

#include "util/json/reader/IResettableReader.h"

#include <string>
#include <sstream>
#include <memory>

namespace util
{
	namespace json
	{
		namespace reader
		{
			class DefaultResettableReader : public IResettableReader
			{
				using InputStreamPtr = std::shared_ptr<std::istringstream>;

			public: // functions

				DefaultResettableReader(const std::string& input);

				DefaultResettableReader(const InputStreamPtr input);

				int32_t read() override;

				void mark(int32_t lookAheadLimit) override;

				void reset() override;

			private: // members

				InputStreamPtr mReader;

				int32_t mMarkedPosition = 0;

				int32_t mLookAheadLimit = -1;
			};
		}
	}
}

#endif //_UTIL_JSON_READER_DEFAULTRESETTABLEREADER_H
