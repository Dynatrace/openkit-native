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

#include "LeafAction.h"
#include "protocol/Beacon.h"
#include "RootAction.h"

#include <atomic>
#include <memory>
#include <sstream>

#include "NullWebRequestTracer.h"
#include "WebRequestTracer.h"

using namespace core::objects;

LeafAction::LeafAction
(
	std::shared_ptr<IActionCommon> actionImpl,
	std::shared_ptr<openkit::IRootAction> parentAction
)
	: mActionImpl(actionImpl)
	, mParentAction(parentAction)
{
}

std::shared_ptr<openkit::IAction> LeafAction::reportEvent(const char* eventName)
{
	mActionImpl->reportEvent(eventName);
	return shared_from_this();
}

std::shared_ptr<openkit::IAction> LeafAction::reportValue(const char* valueName, int32_t value)
{
	mActionImpl->reportValue(valueName, value);
	return shared_from_this();
}

std::shared_ptr<openkit::IAction> LeafAction::reportValue(const char* valueName, int64_t value)
{
	mActionImpl->reportValue(valueName, value);
	return shared_from_this();
}

std::shared_ptr<openkit::IAction> LeafAction::reportValue(const char* valueName, double value)
{
	mActionImpl->reportValue(valueName, value);
	return shared_from_this();
}

std::shared_ptr<openkit::IAction> LeafAction::reportValue(const char* valueName, const char* value)
{
	mActionImpl->reportValue(valueName, value);
	return shared_from_this();
}

std::shared_ptr<openkit::IAction> LeafAction::reportError(const char* errorName, int32_t errorCode, const char* /* reason */)
{
	mActionImpl->reportError(errorName, errorCode);
	return shared_from_this();
}


std::shared_ptr<openkit::IAction> LeafAction::reportError(const char* errorName, int32_t errorCode)
{
	mActionImpl->reportError(errorName, errorCode);
	return shared_from_this();
}

std::shared_ptr<openkit::IAction> LeafAction::reportError(
	const char* errorName,
	const char* causeName,
	const char* causeDescription,
	const char* causeStackTrace
)
{
	mActionImpl->reportError(errorName, causeName, causeDescription, causeStackTrace);
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> LeafAction::traceWebRequest(const char* url)
{
	return mActionImpl->traceWebRequest(url);
}

std::shared_ptr<openkit::IRootAction> LeafAction::leaveAction()
{
	mActionImpl->leaveAction();
	return mParentAction;
}

const std::shared_ptr<IActionCommon> LeafAction::getActionImpl()
{
	return mActionImpl;
}
