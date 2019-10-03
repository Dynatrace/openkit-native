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

#ifndef _PROVIDERS_TYPES_H
#define _PROVIDERS_TYPES_H

#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultPRNGenerator.h"
#include "providers/DefaultSessionIDProvider.h"
#include "providers/DefaultThreadIDProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/IPRNGenerator.h"
#include "providers/ISessionIDProvider.h"
#include "providers/ITimingProvider.h"

namespace test
{
	namespace types
	{
		using DefaultHttpClientProvider_t = providers::DefaultHTTPClientProvider;
		using DefaultHttpClientProvider_sp = std::shared_ptr<DefaultHttpClientProvider_t>;

		using DefaultPrnGenerator_t = providers::DefaultPRNGenerator;
		using DefaultPrnGenerator_sp = std::shared_ptr<DefaultPrnGenerator_t>;

		using DefaultSessionIdProvider_t = providers::DefaultSessionIDProvider;
		using DefaultSessionIdProvider_sp = std::shared_ptr<DefaultSessionIdProvider_t>;

		using DefaultThreadIdProvider_t = providers::DefaultThreadIDProvider;
		using DefaultThreadIdProvider_sp = std::shared_ptr<DefaultThreadIdProvider_t>;

		using DefaultTimingProvider_t = providers::DefaultTimingProvider;
		using DefaultTimingProvider_sp = std::shared_ptr<DefaultTimingProvider_t>;

		using IHttpClientProvider_t = providers::IHTTPClientProvider;
		using IHttpClientProvider_sp = std::shared_ptr<IHttpClientProvider_t>;

		using IPrnGenerator_t = providers::IPRNGenerator;
		using IPrnGenerator_sp = std::shared_ptr<IPrnGenerator_t>;

		using ISessionIdProvider_t = providers::ISessionIDProvider;
		using ISessionIdProvider_sp = std::shared_ptr<ISessionIdProvider_t>;

		using IThreadIdProvider_t = providers::IThreadIDProvider;
		using IThreadIdProvider_sp = std::shared_ptr<IThreadIdProvider_t>;

		using ITimingProvider_t = providers::ITimingProvider;
		using ITimingProvider_sp = std::shared_ptr<ITimingProvider_t>;
	}
}

#endif
