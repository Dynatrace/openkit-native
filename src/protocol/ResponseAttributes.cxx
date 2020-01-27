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

#include "ResponseAttributes.h"
#include "ResponseAttributesDefaults.h"

using namespace protocol;

ResponseAttributes::ResponseAttributes(Builder& builder)
	: mSetAttributes(builder.getSetAttributes())
	, mMaxBeaconSizeInBytes(builder.getMaxBeaconSizeInBytes())
	, mMaxSessionDurationInMilliseconds(builder.getMaxSessionDurationInMilliseconds())
	, mMaxEventsPerSession(builder.getMaxEventsPerSession())
	, mSessionTimeoutInMilliseconds(builder.getSessionTimeoutInMilliseconds())
	, mSendIntervalInMilliseconds(builder.getSendIntervalInMilliseconds())
	, mVisitStoreVersion(builder.getVisitStoreVersion())
	, mIsCapture(builder.isCapture())
	, mIsCaptureCrashes(builder.isCaptureCrashes())
	, mIsCaptureErrors(builder.isCaptureErrors())
	, mMultiplicity(builder.getMultiplicity())
	, mServerId(builder.getServerId())
	, mTimestampInMilliseconds(builder.getTimestampInMilliseconds())
{
}

ResponseAttributes::Builder ResponseAttributes::withKeyValueDefaults()
{
	return ResponseAttributes::Builder(*ResponseAttributesDefaults::KEY_VALUE_RESPONSE);
}

ResponseAttributes::Builder ResponseAttributes::withJsonDefaults()
{
	return ResponseAttributes::Builder(*ResponseAttributesDefaults::JSON_RESPONSE);
}

ResponseAttributes::Builder ResponseAttributes::withUndefinedDefaults()
{
	return ResponseAttributes::Builder(*ResponseAttributesDefaults::UNDEFINED);
}

int32_t ResponseAttributes::getMaxBeaconSizeInBytes() const
{
	return mMaxBeaconSizeInBytes;
}

int32_t ResponseAttributes::getMaxSessionDurationInMilliseconds() const
{
	return mMaxSessionDurationInMilliseconds;
}

int32_t ResponseAttributes::getMaxEventsPerSession() const
{
	return mMaxEventsPerSession;
}

int32_t ResponseAttributes::getSessionTimeoutInMilliseconds() const
{
	return mSessionTimeoutInMilliseconds;
}

int32_t ResponseAttributes::getSendIntervalInMilliseconds() const
{
	return mSendIntervalInMilliseconds;
}

int32_t ResponseAttributes::getVisitStoreVersion() const
{
	return mVisitStoreVersion;
}

bool ResponseAttributes::isCapture() const
{
	return mIsCapture;
}

bool ResponseAttributes::isCaptureCrashes() const
{
	return mIsCaptureCrashes;
}

bool ResponseAttributes::isCaptureErrors() const
{
	return mIsCaptureErrors;
}

int32_t ResponseAttributes::getMultiplicity() const
{
	return mMultiplicity;
}

int32_t ResponseAttributes::getServerId() const
{
	return mServerId;
}

int64_t ResponseAttributes::getTimestampInMilliseconds() const
{
	return mTimestampInMilliseconds;
}

bool ResponseAttributes::isAttributeSet(ResponseAttribute attribute) const
{
	return mSetAttributes.find(attribute) != mSetAttributes.end();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// merge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<IResponseAttributes> ResponseAttributes::merge(std::shared_ptr<IResponseAttributes> attributes) const
{
	auto builder = ResponseAttributes::Builder(*this);

	applyBeaconSize(builder, attributes);
	applySessionDuration(builder, attributes);
	applyEventsPerSession(builder, attributes);
	applySessionTimeout(builder, attributes);
	applySendInterval(builder, attributes);
	applyVisitStoreVersion(builder, attributes);
	applyCapture(builder, attributes);
	applyCaptureCrashes(builder, attributes);
	applyCaptureErrors(builder, attributes);
	applyMultiplicity(builder, attributes);
	applyServerId(builder, attributes);
	applyTimestamp(builder, attributes);

	return builder.build();
}

void ResponseAttributes::applyBeaconSize(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::MAX_BEACON_SIZE))
	{
		builder.withMaxBeaconSizeInBytes(attributes->getMaxBeaconSizeInBytes());
	}
}

void ResponseAttributes::applySessionDuration(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::MAX_SESSION_DURATION))
	{
		builder.withMaxSessionDurationInMilliseconds(attributes->getMaxSessionDurationInMilliseconds());
	}
}

void ResponseAttributes::applyEventsPerSession(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::MAX_EVENTS_PER_SESSION))
	{
		builder.withMaxEventsPerSession(attributes->getMaxEventsPerSession());
	}
}

void ResponseAttributes::applySessionTimeout(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::SESSION_IDLE_TIMEOUT))
	{
		builder.withSessionTimeoutInMilliseconds(attributes->getSessionTimeoutInMilliseconds());
	}
}

void ResponseAttributes::applySendInterval(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::SEND_INTERVAL))
	{
		builder.withSendIntervalInMilliseconds(attributes->getSendIntervalInMilliseconds());
	}
}

void ResponseAttributes::applyVisitStoreVersion(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::VISIT_STORE_VERSION))
	{
		builder.withVisitStoreVersion(attributes->getVisitStoreVersion());
	}
}

void ResponseAttributes::applyCapture(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::IS_CAPTURE))
	{
		builder.withCapture(attributes->isCapture());
	}
}

void ResponseAttributes::applyCaptureCrashes(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::IS_CAPTURE_CRASHES))
	{
		builder.withCaptureCrashes(attributes->isCaptureCrashes());
	}
}

void ResponseAttributes::applyCaptureErrors(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::IS_CAPTURE_ERRORS))
	{
		builder.withCaptureErrors(attributes->isCaptureErrors());
	}
}

void ResponseAttributes::applyMultiplicity(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::MULTIPLICITY))
	{
		builder.withMultiplicity(attributes->getMultiplicity());
	}
}

void ResponseAttributes::applyServerId(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::SERVER_ID))
	{
		builder.withServerId(attributes->getServerId());
	}
}

void ResponseAttributes::applyTimestamp(
	ResponseAttributes::Builder& builder,
	std::shared_ptr<IResponseAttributes> attributes
)
{
	if (attributes->isAttributeSet(ResponseAttribute::TIMESTAMP))
	{
		builder.withTimestampInMilliseconds(attributes->getTimestampInMilliseconds());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Builder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResponseAttributes::Builder::Builder(const IResponseAttributes& defaults)
	: mSetAttributes()
	, mMaxBeaconSizeInBytes(defaults.getMaxBeaconSizeInBytes())
	, mMaxSessionDurationInMilliseconds(defaults.getMaxSessionDurationInMilliseconds())
	, mMaxEventsPerSession(defaults.getMaxEventsPerSession())
	, mSessionTimeoutInMilliseconds(defaults.getSessionTimeoutInMilliseconds())
	, mSendIntervalInMilliseconds(defaults.getSendIntervalInMilliseconds())
	, mVisitStoreVersion(defaults.getVisitStoreVersion())
	, mIsCapture(defaults.isCapture())
	, mIsCaptureCrashes(defaults.isCaptureCrashes())
	, mIsCaptureErrors(defaults.isCaptureErrors())
	, mMultiplicity(defaults.getMultiplicity())
	, mServerId(defaults.getServerId())
	, mTimestampInMilliseconds(defaults.getTimestampInMilliseconds())
{
	for (const auto attribute : ALL_RESPONSE_ATTRIBUTES)
	{
		if (defaults.isAttributeSet(attribute)) {
			setAttribute(attribute);
		}
	}
}

int32_t ResponseAttributes::Builder::getMaxBeaconSizeInBytes() const
{
	return mMaxBeaconSizeInBytes;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withMaxBeaconSizeInBytes(int32_t maxBeaconSizeInBytes)
{
	mMaxBeaconSizeInBytes = maxBeaconSizeInBytes;
	setAttribute(ResponseAttribute::MAX_BEACON_SIZE);
	return *this;
}

int32_t ResponseAttributes::Builder::getMaxSessionDurationInMilliseconds() const
{
	return mMaxSessionDurationInMilliseconds;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withMaxSessionDurationInMilliseconds(
	int32_t maxSessionDurationInMilliseconds
)
{
	mMaxSessionDurationInMilliseconds = maxSessionDurationInMilliseconds;
	setAttribute(ResponseAttribute::MAX_SESSION_DURATION);
	return *this;
}

int32_t ResponseAttributes::Builder::getMaxEventsPerSession() const
{
	return mMaxEventsPerSession;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withMaxEventsPerSession(int32_t maxEventsPerSession)
{
	mMaxEventsPerSession = maxEventsPerSession;
	setAttribute(ResponseAttribute::MAX_EVENTS_PER_SESSION);
	return *this;
}

int32_t ResponseAttributes::Builder::getSessionTimeoutInMilliseconds() const
{
	return mSessionTimeoutInMilliseconds;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withSessionTimeoutInMilliseconds(
	int32_t sessionTimeoutInMilliseconds
)
{
	mSessionTimeoutInMilliseconds = sessionTimeoutInMilliseconds;
	setAttribute(ResponseAttribute::SESSION_IDLE_TIMEOUT);
	return *this;
}

int32_t ResponseAttributes::Builder::getSendIntervalInMilliseconds() const
{
	return mSendIntervalInMilliseconds;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withSendIntervalInMilliseconds(
	int32_t sendIntervalInMilliseconds
)
{
	mSendIntervalInMilliseconds = sendIntervalInMilliseconds;
	setAttribute(ResponseAttribute::SEND_INTERVAL);
	return *this;
}

int32_t ResponseAttributes::Builder::getVisitStoreVersion() const
{
	return mVisitStoreVersion;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withVisitStoreVersion(int32_t visitStoreVersion)
{
	mVisitStoreVersion = visitStoreVersion;
	setAttribute(ResponseAttribute::VISIT_STORE_VERSION);
	return *this;
}

bool ResponseAttributes::Builder::isCapture() const
{
	return mIsCapture;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withCapture(bool capture)
{
	mIsCapture = capture;
	setAttribute(ResponseAttribute::IS_CAPTURE);
	return *this;
}

bool ResponseAttributes::Builder::isCaptureCrashes() const
{
	return mIsCaptureCrashes;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withCaptureCrashes(bool captureCrashes)
{
	mIsCaptureCrashes = captureCrashes;
	setAttribute(ResponseAttribute::IS_CAPTURE_CRASHES);
	return *this;
}

bool ResponseAttributes::Builder::isCaptureErrors() const
{
	return mIsCaptureErrors;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withCaptureErrors(bool captureErrors)
{
	mIsCaptureErrors = captureErrors;
	setAttribute(ResponseAttribute::IS_CAPTURE_ERRORS);
	return *this;
}

int32_t ResponseAttributes::Builder::getMultiplicity() const
{
	return mMultiplicity;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withMultiplicity(int32_t multiplicity)
{
	mMultiplicity = multiplicity;
	setAttribute(ResponseAttribute::MULTIPLICITY);
	return *this;
}

int32_t ResponseAttributes::Builder::getServerId() const
{
	return mServerId;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withServerId(int32_t serverId)
{
	mServerId = serverId;
	setAttribute(ResponseAttribute::SERVER_ID);
	return *this;
}

int64_t ResponseAttributes::Builder::getTimestampInMilliseconds() const
{
	return mTimestampInMilliseconds;
}

ResponseAttributes::Builder& ResponseAttributes::Builder::withTimestampInMilliseconds(int64_t timestampInMilliseconds)
{
	mTimestampInMilliseconds = timestampInMilliseconds;
	setAttribute(ResponseAttribute::TIMESTAMP);
	return *this;
}

std::shared_ptr<IResponseAttributes> ResponseAttributes::Builder::build()
{
	return std::make_shared<ResponseAttributes>(*this);
}

ResponseAttributes::AttributeSet& ResponseAttributes::Builder::getSetAttributes()
{
	return mSetAttributes;
}

void ResponseAttributes::Builder::setAttribute(ResponseAttribute attribute)
{
	mSetAttributes.insert(attribute);
}