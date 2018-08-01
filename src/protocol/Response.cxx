/**
* Copyright 2018 Dynatrace LLC
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

#include "Response.h"

using namespace protocol;

static constexpr int32_t HTTP_BAD_REQUEST = 400;

Response::Response(int32_t responseCode, const ResponseHeaders& responseHeaders)
	: mResponseCode(responseCode)
	, mResponseHeaders(responseHeaders)
{
}

bool Response::isErroneousResponse() const
{
	return getResponseCode() >= HTTP_BAD_REQUEST;
}

int32_t Response::getResponseCode() const
{
	return mResponseCode;
}

const Response::ResponseHeaders& Response::getResponseHeaders() const
{
	return mResponseHeaders;
}
