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

#ifndef _API_C_EXTERNALHTTPREQUESTINTERCEPTOR_H
#define _API_C_EXTERNALHTTPREQUESTINTERCEPTOR_H

#include <OpenKit/OpenKit-c.h>
#include <OpenKit/IHttpRequestInterceptor.h>

namespace apic
{
	class CustomHttpRequestInterceptor
		: public openkit::IHttpRequestInterceptor
	{
	public:

		CustomHttpRequestInterceptor(openKitInterceptHttpRequestFunc interceptHttpRequestFunc);

		~CustomHttpRequestInterceptor() override = default;

		void intercept(openkit::IHttpRequest& httpRequest) override;
		
	private:

		openKitInterceptHttpRequestFunc mInterceptHttpRequestFunc;
	};
}

#endif
