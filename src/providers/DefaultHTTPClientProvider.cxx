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

#include "DefaultHTTPClientProvider.h"
#include "protocol/HTTPClient.h"

using namespace providers;

DefaultHTTPClientProvider::DefaultHTTPClientProvider(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender
)
	: mLogger(logger)
	, mThreadSuspender(threadSuspender)
{
}

std::shared_ptr<protocol::IHTTPClient> DefaultHTTPClientProvider::createClient(
	std::shared_ptr<core::configuration::IHTTPClientConfiguration> configuration
)
{
	return std::make_shared<protocol::HTTPClient>(mLogger, configuration, mThreadSuspender);
}