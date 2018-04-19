/**
* Copyright 2018 Dynatrace LLC
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

#include "WebRequestTracerBase.h"

#include "protocol/Beacon.h"

namespace core
{
	WebRequestTracerBase::WebRequestTracerBase(std::shared_ptr<protocol::Beacon> beacon, int32_t parentActionID)
		: mBeacon(beacon)
		, mParentActionID(parentActionID)
		, mResponseCode(-1)
		, mBytesSent(-1)
		, mBytesReceived(-1)
		, mStartTime(beacon->getCurrentTimestamp())//if start is not called by the user a meaningful default close to the actual timing is possible
		, mEndTime(-1)
		, mStartSequenceNo(beacon->createSequenceNumber())
		, mEndSequenceNo(-1)
		, mWebRequestTag(beacon->createTag(parentActionID, mStartSequenceNo))
		, mURL(core::UTF8String("<unknown>"))
	{

	}

	const char* WebRequestTracerBase::getTag() const
	{
		return mWebRequestTag.getStringData().c_str();
	}

	std::shared_ptr<openkit::IWebRequestTracer> WebRequestTracerBase::setResponseCode(int32_t responseCode)
	{
		if (!isStopped())
		{
			mResponseCode = responseCode;
		}
		return shared_from_this();
	}

	std::shared_ptr<openkit::IWebRequestTracer> WebRequestTracerBase::setBytesSent(int32_t bytesSent)
	{
		if (!isStopped())
		{
			mBytesSent = bytesSent;
		}
		return shared_from_this();
	}

	std::shared_ptr<openkit::IWebRequestTracer> WebRequestTracerBase::setBytesReceived(int32_t bytesReceived)
	{
		if (!isStopped())
		{
			mBytesReceived = bytesReceived;
		}
		return shared_from_this();
	}

	std::shared_ptr<openkit::IWebRequestTracer> WebRequestTracerBase::start()
	{
		if (!isStopped())
		{
			mStartTime = mBeacon->getCurrentTimestamp();
		}
		return shared_from_this();
	}

	void WebRequestTracerBase::stop()
	{
		int64_t expected = -1;
		if (atomic_compare_exchange_strong(&mEndTime, &expected, mBeacon->getCurrentTimestamp()) == false)
		{
			return;
		}

		mEndSequenceNo = mBeacon->createSequenceNumber();

		//add web request to beacon
		mBeacon->addWebRequest(mParentActionID, shared_from_this());
	}

	const core::UTF8String WebRequestTracerBase::getURL() const
	{
		return mURL;
	}

	int32_t WebRequestTracerBase::getResponseCode() const
	{
		return mResponseCode;
	}

	int64_t WebRequestTracerBase::getStartTime() const
	{
		return mStartTime;
	}

	int64_t WebRequestTracerBase::getEndTime() const
	{
		return mEndTime;
	}

	int32_t WebRequestTracerBase::getStartSequenceNo() const
	{
		return mStartSequenceNo;
	}

	int32_t WebRequestTracerBase::getEndSequenceNo() const
	{
		return mEndSequenceNo;
	}

	int32_t WebRequestTracerBase::getBytesSent() const
	{
		return mBytesSent;
	}

	int32_t WebRequestTracerBase::getBytesReceived() const
	{
		return mBytesReceived;
	}

	bool WebRequestTracerBase::isStopped() const
	{
		return mEndTime != -1;
	}

}