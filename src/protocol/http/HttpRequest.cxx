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

#include "HttpRequest.h"
#include "HttpHeaderUtil.h"

#include <unordered_set>

using namespace protocol;

using RestrictedHeaderContainerType = std::unordered_set < std::string, HeaderNameHasher, HeaderNameEqualityComparer>;
static const RestrictedHeaderContainerType RESTRICTED_HEADERS =
{
	"Access-Control-Request-Headers",
	"Access-Control-Request-Method",
	"Connection",
	"Content-Length",
	"Content-Transfer-Encoding",
	"Host",
	"Keep-Alive",
	"Origin",
	"Trailer",
	"Transfer-Encoding",
	"Upgrade",
	"Via"
};

HttpRequest::HttpRequest(const std::string& uri, const std::string& method)
	: mUri(uri)
	, mMethod(method)
	, mHeaders()
{
}

const std::string& HttpRequest::getUri() const
{
	return mUri;
}

const std::string& HttpRequest::getMethod() const
{
	return mMethod;
}

bool HttpRequest::containsHeader(const std::string& name) const
{
	return mHeaders.contains(name);
}

const std::string& HttpRequest::getHeader(const std::string& name) const
{
	if (!containsHeader(name))
	{
		static const std::string emptyHeader;
		return emptyHeader;
	}

	return mHeaders.getHeader(name).front();
}

void HttpRequest::setHeader(const std::string& name, const std::string& value)
{
	if (isHeaderRestricted(name))
	{
		return;
	}

	mHeaders.setHeader(name, value);
}

const HttpHeaderCollection& HttpRequest::getHttpHeaders() const
{
	return mHeaders;
}

bool HttpRequest::isHeaderRestricted(const std::string& name)
{
	return RESTRICTED_HEADERS.find(name) != RESTRICTED_HEADERS.end();
}
