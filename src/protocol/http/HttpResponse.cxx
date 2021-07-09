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

#include "HttpResponse.h"

using namespace protocol;

HttpResponse::HttpResponse(
	const std::string& requestUri,
	const std::string& requestMethod,
	int32_t statusCode,
	const std::string& reasonPhrase,
	const HttpHeaderCollection& responseHeaders,
	const std::string& responseBody
)
	: mRequestUri(requestUri)
	, mRequestMethod(requestMethod)
	, mStatusCode(statusCode)
	, mReasonPhrase(reasonPhrase)
	, mResponseHeaders(responseHeaders)
	, mResponseBody(responseBody)
{
}

const std::string& HttpResponse::getRequestUri() const
{
	return mRequestUri;
}

const std::string& HttpResponse::getRequestMethod() const
{
	return mRequestMethod;
}

int32_t HttpResponse::getStatusCode() const
{
	return mStatusCode;
}

const std::string& HttpResponse::getReasonPhrase() const
{
	return mReasonPhrase;
}

bool HttpResponse::containsHeader(const std::string& name) const
{
	return mResponseHeaders.contains(name);
}

const std::list<std::string>& HttpResponse::getHeader(const std::string& name) const
{
	if (!containsHeader(name))
	{
		static const std::list<std::string> emptyHeader;
		return emptyHeader;
	}

	return mResponseHeaders.getHeader(name);
}

const std::string& HttpResponse::getResponseBody() const
{
	return mResponseBody;
}

const HttpHeaderCollection& HttpResponse::getResponseHeaders() const
{
	return mResponseHeaders;
}
