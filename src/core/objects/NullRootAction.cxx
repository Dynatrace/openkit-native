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

#include "NullRootAction.h"
#include "NullAction.h"

using namespace core::objects;

const std::shared_ptr<NullRootAction> NullRootAction::instance()
{
	static const auto instance = std::make_shared<NullRootAction>();

	return instance;
}

std::shared_ptr<openkit::IAction> NullRootAction::enterAction(const char* /*actionName*/)
{
	return std::make_shared<NullAction>(shared_from_this());
}

std::shared_ptr<openkit::IRootAction> NullRootAction::reportEvent(const char* /*eventName*/)
{
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> NullRootAction::reportValue(const char* /*valueName*/, int32_t /*value*/)
{
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> NullRootAction::reportValue(const char* /*valueName*/, int64_t /*value*/)
{
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> NullRootAction::reportValue(const char* /*valueName*/, double /*value*/)
{
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> NullRootAction::reportValue(const char* /*valueName*/, const char* /*value*/)
{
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> NullRootAction::reportError(const char* /*errorName*/, int32_t /*errorCode*/, const char* /*reason*/)
{
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> NullRootAction::traceWebRequest(const char* /*url*/)
{
	return NullWebRequestTracer::instance();
}

void NullRootAction::leaveAction()
{
	// intentionally left empty, due to NullObject pattern
}