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

#include "util/json/reader/DefaultResettableReader.h"

using namespace util::json::reader;


DefaultResettableReader::DefaultResettableReader(const std::string& input)
	: DefaultResettableReader(std::make_shared<std::istringstream>(input))
{
}

DefaultResettableReader::DefaultResettableReader(const InputStreamPtr input)
	: mReader(input)
{
}

int32_t DefaultResettableReader::read()
{
	if (mReader->eof())
	{
		return EOF;
	}

	char character = EOF;
	mReader->get(character);

	return character;
}

void DefaultResettableReader::mark(int32_t lookAheadLimit)
{
	if (lookAheadLimit < 0)
	{
		throw std::invalid_argument("lookAheadLimit < 0");
	}
	mLookAheadLimit = lookAheadLimit;
	mMarkedPosition = mReader->tellg();
}

void DefaultResettableReader::reset()
{
	if (mLookAheadLimit < 0)
	{
		throw std::logic_error("No position has been marked");
	}

	auto currentPos = mReader->tellg();

	if (currentPos > mMarkedPosition + mLookAheadLimit)
	{
		std::ostringstream os;
		os << "Cannot reset beyond " << mLookAheadLimit << " characters. Tried to reset "
			<< (currentPos - mMarkedPosition) << " characters";
		throw std::logic_error(os.str());
	}

	mReader->seekg(mMarkedPosition);
}