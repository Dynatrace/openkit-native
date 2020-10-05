/**
 * Copyright 2018-2020 Dynatrace LLC
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

#include "NullSession.h"
#include "NullRootAction.h"
#include "NullWebRequestTracer.h"

using namespace core::objects;

const std::shared_ptr<NullSession> NullSession::instance()
{
	static const auto instance = std::make_shared<NullSession>();

	return instance;
}

std::shared_ptr<openkit::IRootAction> NullSession::enterAction(const char* /*actionName*/)
{
	return NullRootAction::instance();
}

void NullSession::identifyUser(const char* /*userTag*/)
{
	// intentionally left empty, due to NullObject pattern
}

void NullSession::reportCrash(const char* /*errorName*/, const char* /*reason*/, const char* /*stacktrace*/)
{
	// intentionally left empty, due to NullObject pattern
}

std::shared_ptr<openkit::IWebRequestTracer> NullSession::traceWebRequest(const char* /*url*/)
{
	return NullWebRequestTracer::instance();
}

void NullSession::end()
{
	// intentionally left empty, due to NullObject pattern
}