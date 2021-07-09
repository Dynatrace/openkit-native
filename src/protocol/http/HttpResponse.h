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

#ifndef _PROTOCOL_HTTP_HTTPRESPONSE_H
#define _PROTOCOL_HTTP_HTTPRESPONSE_H

#include <OpenKit/IHttpResponse.h>

#include "HttpHeaderCollection.h"

namespace protocol
{
	class HttpResponse : public openkit::IHttpResponse
	{
	public:

		HttpResponse(
			const std::string& requestUri,
			const std::string& requestMethod,
			int32_t statusCode,
			const std::string& reasonPhrase,
			const HttpHeaderCollection& responseHeaders,
			const std::string& responseBody
		);

		~HttpResponse() override = default;

		const std::string& getRequestUri() const override;

		const std::string& getRequestMethod() const override;

		int32_t getStatusCode() const override;

		const std::string& getReasonPhrase() const override;

		bool containsHeader(const std::string& name) const override;

		const std::list<std::string>& getHeader(const std::string& name) const override;

		const HttpHeaderCollection& getResponseHeaders() const;

		const std::string& getResponseBody() const;

	private:

		std::string mRequestUri;
		std::string mRequestMethod;
		int32_t mStatusCode;
		std::string mReasonPhrase;
		HttpHeaderCollection mResponseHeaders;
		std::string mResponseBody;
	};
}

#endif
