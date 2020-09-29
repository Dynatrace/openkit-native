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

#include "RootAction.h"

#include <memory>
#include <sstream>

#include "NullWebRequestTracer.h"

using namespace core::objects;

RootAction::RootAction
(
	std::shared_ptr<IActionCommon> actionImpl
)
	: mActionImpl(actionImpl)
{
}

std::shared_ptr<openkit::IAction> RootAction::enterAction(const char* actionName)
{
	return mActionImpl->enterAction(shared_from_this(), actionName);
}

std::shared_ptr<openkit::IRootAction> RootAction::reportEvent(const char* eventName)
{
	mActionImpl->reportEvent(eventName);
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> RootAction::reportValue(const char* valueName, int32_t value)
{
	mActionImpl->reportValue(valueName, value);
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> RootAction::reportValue(const char* valueName, double value)
{
	mActionImpl->reportValue(valueName, value);
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> RootAction::reportValue(const char* valueName, const char* value)
{
	mActionImpl->reportValue(valueName, value);
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> RootAction::reportError(const char* errorName, int32_t errorCode, const char* reason)
{
	mActionImpl->reportError(errorName, errorCode, reason);
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> RootAction::traceWebRequest(const char* url)
{
	return mActionImpl->traceWebRequest(url);
}

void RootAction::leaveAction()
{
	mActionImpl->leaveAction();
}

std::shared_ptr<IActionCommon> RootAction::getActionImpl()
{
	return mActionImpl;
}
