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

#include "WebRequestTracer.h"

#include "protocol/IBeacon.h"

#include <sstream>
#include <regex>

using namespace core::objects;

static const std::regex SCHEMA_VALIDATION_PATTERN("^[a-zA-Z][a-zA-Z0-9+\\-.]*://.+$", std::regex::optimize | std::regex::ECMAScript);

WebRequestTracer::WebRequestTracer
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IOpenKitComposite> parent,
	std::shared_ptr<protocol::IBeacon> beacon,
	const core::UTF8String& url
)
	: mLogger(logger)
	, mParent(parent)
	, mMutex()
	, mBeacon(beacon)
	, mParentActionID(parent->getActionId())
	, mURL(calculateUrlFrom(url))
	, mResponseCode(-1)
	, mBytesSent(-1)
	, mBytesReceived(-1)
	, mStartTime(beacon->getCurrentTimestamp())//if start is not called by the user a meaningful default close to the actual timing is possible
	, mEndTime(-1)
	, mStartSequenceNo(beacon->createSequenceNumber())
	, mEndSequenceNo(-1)
	, mWebRequestTag(beacon->createTag(parent->getActionId(), mStartSequenceNo))
{
}

bool WebRequestTracer::isValidURLScheme(const core::UTF8String& url)
{
	return std::regex_match(url.getStringData(), SCHEMA_VALIDATION_PATTERN);
}

core::UTF8String WebRequestTracer::calculateUrlFrom(const core::UTF8String& url)
{
	if (!isValidURLScheme(url))
	{
		return UNKNOWN_URL;
	}

	auto indexOfQuestionMark = url.getIndexOf("?");

	if (indexOfQuestionMark == std::string::npos)
	{
		return url;
	}

	return url.substring(0, indexOfQuestionMark);
}

const char* WebRequestTracer::getTag() const
{
	const char* tag = mWebRequestTag.getStringData().c_str();
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s getTag() returning '%s'", toString().c_str(), tag);
	}
	return tag;
}

std::shared_ptr<openkit::IWebRequestTracer> WebRequestTracer::setBytesSent(int32_t bytesSent)
{
	// synchronized scope
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (!isStopped())
		{
			mBytesSent = bytesSent;
		}
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> WebRequestTracer::setBytesReceived(int32_t bytesReceived)
{
	// synchronized scope
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (!isStopped())
		{
			mBytesReceived = bytesReceived;
		}
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> WebRequestTracer::start()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s - start()", toString().c_str());
	}

	// synchronized scope
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (!isStopped())
		{
			mStartTime = mBeacon->getCurrentTimestamp();
		}
	}
	return shared_from_this();
}

void WebRequestTracer::stop(int32_t responseCode)
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s - stop(rc=%d)", toString().c_str(), responseCode);
	}

	doStop(responseCode, false);
}

void WebRequestTracer::doStop(int32_t responseCode, bool discardData)
{
	// synchronized scope
	{
		std::lock_guard<std::mutex> lock(mMutex);
		if (isStopped()) {
			// stop was already previously called.
			return;
		}

		mResponseCode = responseCode;
		mEndSequenceNo = mBeacon->createSequenceNumber();
		mEndTime = mBeacon->getCurrentTimestamp();
	}

	if (!discardData)
	{
		//add web request to beacon
		mBeacon->addWebRequest(mParentActionID, shared_from_this());
	}

	// detach from parent
	mParent->onChildClosed(shared_from_this());
}

void WebRequestTracer::close()
{
	stop(mResponseCode);
}

void WebRequestTracer::cancel()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s - cancel()", toString().c_str());
	}

	doStop(mResponseCode, true);
}

const core::UTF8String& WebRequestTracer::getURL() const
{
	return mURL;
}

int32_t WebRequestTracer::getResponseCode() const
{
	return mResponseCode;
}

int64_t WebRequestTracer::getStartTime() const
{
	return mStartTime;
}

int64_t WebRequestTracer::getEndTime() const
{
	return mEndTime;
}

int32_t WebRequestTracer::getStartSequenceNo() const
{
	return mStartSequenceNo;
}

int32_t WebRequestTracer::getEndSequenceNo() const
{
	return mEndSequenceNo;
}

int32_t WebRequestTracer::getBytesSent() const
{
	return mBytesSent;
}

int32_t WebRequestTracer::getBytesReceived() const
{
	return mBytesReceived;
}

bool WebRequestTracer::isStopped() const
{
	return mEndTime != -1;
}

const std::string WebRequestTracer::toString() const
{
	std::stringstream ss;
	ss << "WebRequestTracer [sn=" << this->mBeacon->getSessionNumber() << ", id=" << mParentActionID << ", url='" << mURL.getStringData().c_str() << "']";
	return ss.str();
}
