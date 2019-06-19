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

#include "HTTPResponseParser.h"

#include <cctype>
#include <algorithm>

using namespace protocol;

static constexpr char HTTP_HEADER_LINE_KEY_VALUE_SEPARATOR = ':';
static constexpr char HTTP_HEADER_LINE_VALUE_SEPARATOR = ',';

HTTPResponseParser::HTTPResponseParser()
	: mResponseHeaders()
	, mResponseBody()
{
}


size_t HTTPResponseParser::responseHeaderData(const char *buffer, size_t elementSize, size_t numberOfElements)
{
	// convert response line into STL string
	auto responseLine = std::string(buffer, elementSize * numberOfElements);

	// split up response header line
	auto separatorPosition = responseLine.find(HTTP_HEADER_LINE_KEY_VALUE_SEPARATOR);
	if (separatorPosition != std::string::npos)
	{
		// found the separator - split into key and value
		auto key = responseLine.substr(0, separatorPosition);
		auto valueString = responseLine.substr(separatorPosition + 1);

		// strip optional whitespace character
		HTTPResponseParser::stripWhitespaces(valueString);

		// key is case insensitive
		std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		auto responseKeyIterator = mResponseHeaders.find(key);
		if (responseKeyIterator == mResponseHeaders.end())
		{
			// key not yet present
			responseKeyIterator = mResponseHeaders.insert({ key, std::vector<std::string>() }).first;
		}

		// move over previously parsed values
		responseKeyIterator->second.push_back(valueString);
	}

	// in any case return the number of bytes processed
	return elementSize * numberOfElements;
}

size_t HTTPResponseParser::responseBodyData(const char* buffer, size_t elementSize, size_t numberOfElements)
{
	mResponseBody.append(buffer, elementSize * numberOfElements);
	return elementSize * numberOfElements;
}

const Response::ResponseHeaders& HTTPResponseParser::getResponseHeaders() const
{
	return mResponseHeaders;
}

const std::string& HTTPResponseParser::getResponseBody() const
{
	return mResponseBody;
}

bool HTTPResponseParser::isWhitespace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void HTTPResponseParser::stripWhitespaces(std::string& str)
{
	auto begin = str.cbegin();
	auto end = str.cend();

	// strip leading whitespace
	while (begin < end && isWhitespace(*begin))
	{
		begin++;
	}
	// strip trailing whitespaces
	end -= 1;
	while (end > begin && isWhitespace(*end))
	{
		end--;
	}

	end += 1;
	
	// assign final result
	str = end > begin ? std::string(begin, end) : std::string();
}
