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

#ifndef _PROTOCOL_HTTP_HTTPREQUEST_H
#define _PROTOCOL_HTTP_HTTPREQUEST_H

#include <OpenKit/IHttpRequest.h>

#include "HttpHeaderCollection.h"

namespace protocol
{
	class HttpRequest : public openkit::IHttpRequest
	{
	public:

		HttpRequest(const std::string& uri, const std::string& method);

		~HttpRequest() override = default;

		const std::string& getUri() const override;

		const std::string& getMethod() const override;

		std::list<std::string> getHeaderNames() const override;

		bool containsHeader(const std::string& name) const override;

		const std::string& getHeader(const std::string& name) const override;

		void setHeader(const std::string& name, const std::string& value) override;

		const HttpHeaderCollection& getHttpHeaders() const;

	private:

		static bool isHeaderRestricted(const std::string& name);

		std::string mUri;
		std::string mMethod;
		HttpHeaderCollection mHeaders;
	};
}

#endif
