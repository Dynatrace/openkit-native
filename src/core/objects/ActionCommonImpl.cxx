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

#include "ActionCommonImpl.h"
#include "core/UTF8String.h"
#include "protocol/Beacon.h"
#include "WebRequestTracer.h"

using namespace core::objects;

std::shared_ptr<NullWebRequestTracer> ActionCommonImpl::NULL_WEB_REQUEST_TRACER(std::make_shared<NullWebRequestTracer>());

ActionCommonImpl::ActionCommonImpl
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<protocol::Beacon> beacon,
	int32_t actionID,
	const std::string& objectID
)
	: mLogger(logger)
	, mBeacon(beacon)
	, mActionID(actionID)
	, mObjectID(objectID)
{
}

void ActionCommonImpl::reportEvent(const char* eventName)
{
	UTF8String eventNameString(eventName);
	if (eventNameString.empty())
	{
		mLogger->warning("%s reportEvent: eventName must not be null or empty", mObjectID.c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportEvent(%s)", mObjectID.c_str(), eventNameString.getStringData().c_str());
	}

	mBeacon->reportEvent(mActionID, eventNameString);
}

void ActionCommonImpl::reportValue(const char* valueName, int32_t value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		mLogger->warning("%s reportValue (int): valueName must not be null or empty", mObjectID.c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportValue (int) (%s, %d))", mObjectID.c_str(), valueNameString.getStringData().c_str(), value);
	}

	mBeacon->reportValue(mActionID, valueNameString, value);

}

void ActionCommonImpl::reportValue(const char* valueName, double value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		mLogger->warning("%s reportValue (double): valueName must not be null or empty", mObjectID.c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportValue (double) (%s, %f))", mObjectID.c_str(), valueNameString.getStringData().c_str(), value);
	}

	mBeacon->reportValue(mActionID, valueNameString, value);
}

void ActionCommonImpl::reportValue(const char* valueName, const char* value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		mLogger->warning("%s reportValue (string): valueName must not be null or empty", mObjectID.c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		UTF8String valueString(value);
		mLogger->debug("%s reportValue (string) (%s, %s))", mObjectID.c_str(),
				valueNameString.getStringData().c_str(),
				(value != nullptr ? valueString.getStringData().c_str() : "null"));
	}

	mBeacon->reportValue(mActionID, valueNameString, value);
}


void ActionCommonImpl::reportError(const char* errorName, int32_t errorCode, const char* reason)
{
	UTF8String errorNameString(errorName);
	UTF8String reasonString(reason);
	if (errorNameString.empty())
	{
		mLogger->warning("%s reportError: errorName must not be null or empty", mObjectID.c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportError (%s, %d, %s))", mObjectID.c_str(),
				errorNameString.getStringData().c_str(), errorCode,
				(reason != nullptr ? reasonString.getStringData().c_str() : "null"));
	}

	mBeacon->reportError(mActionID, errorNameString, errorCode, reasonString);

}

std::shared_ptr<openkit::IWebRequestTracer> ActionCommonImpl::traceWebRequest(const std::shared_ptr<OpenKitComposite> parent, const char* url)
{
	core::UTF8String urlString(url);
	if (urlString.empty())
	{
		mLogger->warning("%s traceWebRequest (string): url must not be null or empty", mObjectID.c_str());
		return NULL_WEB_REQUEST_TRACER;
	}
	if (!WebRequestTracer::isValidURLScheme(urlString))
	{
		mLogger->warning("%s traceWebRequest (string): url \"%s\" does not have a valid scheme", mObjectID.c_str(), urlString.getStringData().c_str());
		return NULL_WEB_REQUEST_TRACER;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s traceWebRequest (string) (%s))", mObjectID.c_str(), urlString.getStringData().c_str());
	}

	return std::make_shared<core::objects::WebRequestTracer>(mLogger, parent, mBeacon, urlString);
}