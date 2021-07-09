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

#include "HTTPResponseParser.h"

#include <cctype>
#include <cstring>

using namespace protocol;

static constexpr char HTTP_HEADER_LINE_KEY_VALUE_SEPARATOR = ':';
static constexpr int32_t INITIAL_RESPONSE_STATUS = -1;

HTTPResponseParser::HTTPResponseParser()
	: mResponseStatus(INITIAL_RESPONSE_STATUS)
	, mReasonPhrase()
	, mResponseHeaders()
	, mResponseBody()
{
}

size_t HTTPResponseParser::responseHeaderData(const char *buffer, size_t elementSize, size_t numberOfElements)
{
	auto bufferSizeInBytes = elementSize * numberOfElements;

	const char* start = buffer;
	const char* end = buffer + bufferSizeInBytes;

	// trim leading spaces
	for (; start < end && std::isspace(*start); start++)
		;

	// trim trailing spaces
	for (end = end - 1; start < end && std::isspace(*end); end--)
		;
	end++; // like STL iterator, end is the first non-valid element

	if (start < end)
	{
		// check if it's a header line or the status line
		const char* colon;
		for (colon = start; colon < end && *colon != HTTP_HEADER_LINE_KEY_VALUE_SEPARATOR; colon++)
			;

		if (colon != end)
		{
			// it's an HTTP header line
			if (colon != start) // empty header name is not valid
			{
				std::string headerName(start, colon - start);

				// strip optional whitespace after colon
				const char* valueStart = colon + 1;
				for (; valueStart < end && std::isspace(*valueStart); valueStart++)
					;

				// header value can be empty
				auto headerValue = valueStart < end ? std::string(valueStart, end - valueStart) : std::string();

				mResponseHeaders.appendHeader(headerName, headerValue);
			}
		}
		else
		{
			// should be the HTTP status line - try to parse it as such
			// e.g. HTTP/1.1 200 OK
			// e.g. HTTP/1.1 200
			const char* statusCodeStart;
			// skip HTTP protocol version
			for (statusCodeStart = start; statusCodeStart < end && !std::isspace(*statusCodeStart); statusCodeStart++)
				;
			// skip space - protocol says one, but skip arbitrary number
			for (; statusCodeStart < end && std::isspace(*statusCodeStart); statusCodeStart++)
				;
			// now we should have 3 digits + space + reason phrase
			// where space + reason phrase are optional (and omitted in HTTP/2)
			auto remainingBytes = end - statusCodeStart;
			if (remainingBytes >= 3
				&& std::isdigit(*statusCodeStart)
				&& std::isdigit(*(statusCodeStart + 1))
				&& std::isdigit(*(statusCodeStart + 2))
				&& (remainingBytes == 3 || std::isspace(*(statusCodeStart + 3)))
			)
			{
				// reset parser, so that we only parse the last response
				reset();

				mResponseStatus = (*statusCodeStart - '0') * 100;
				mResponseStatus += (*(statusCodeStart + 1) - '0') * 10;
				mResponseStatus += (*(statusCodeStart + 2) - '0');

				// try parse reason phrase
				const char* reasonPhraseStart;
				for (reasonPhraseStart = statusCodeStart + 3; reasonPhraseStart < end && std::isspace(*reasonPhraseStart); reasonPhraseStart++)
					;

				mReasonPhrase = reasonPhraseStart < end ? std::string(reasonPhraseStart, end - reasonPhraseStart) : std::string();
			}
		}
	}
	// the else is omitted here, but is valid
	// it's very likely the line delimiting header and body data, which only "\r\n".

	// always return that we consumed all bytes, otherwise libcurl aborts and reports an error
	return bufferSizeInBytes;
}

size_t HTTPResponseParser::responseBodyData(const char* buffer, size_t elementSize, size_t numberOfElements)
{
	mResponseBody.append(buffer, elementSize * numberOfElements);
	return elementSize * numberOfElements;
}

int32_t HTTPResponseParser::getResponseStatus() const
{
	return mResponseStatus;
}

const std::string& HTTPResponseParser::getReasonPhrase() const
{
	return mReasonPhrase;
}

const HttpHeaderCollection& HTTPResponseParser::getResponseHeaders() const
{
	return mResponseHeaders;
}

const std::string& HTTPResponseParser::getResponseBody() const
{
	return mResponseBody;
}

void HTTPResponseParser::reset()
{
	mResponseStatus = INITIAL_RESPONSE_STATUS;
	mReasonPhrase.clear();
	mResponseHeaders.clear();
	mResponseBody.clear();
}