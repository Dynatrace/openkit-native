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

#ifndef _PROTOCOL_HTTP_NULLHTTPREQUESTINTERCEPTOR_H
#define _PROTOCOL_HTTP_NULLHTTPREQUESTINTERCEPTOR_H

#include <OpenKit/IHttpRequestInterceptor.h>

#include <memory>

namespace protocol
{
	class NullHttpRequestInterceptor : public openkit::IHttpRequestInterceptor
	{
	public:

		virtual ~NullHttpRequestInterceptor() override = default;

		static std::shared_ptr<NullHttpRequestInterceptor> instance();

		virtual void intercept(openkit::IHttpRequest& httpRequest) override;
	};
}

#endif
