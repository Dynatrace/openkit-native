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

#include "SessionCreator.h"

#include "core/configuration/BeaconConfiguration.h"
#include "core/objects/Session.h"
#include "protocol/Beacon.h"
#include "providers/FixedPRNGenerator.h"
#include "providers/FixedSessionIDProvider.h"
#include "providers/DefaultPRNGenerator.h"

using namespace core::objects;

SessionCreator::SessionCreator(ISessionCreatorInput& sessionCreatorInput, const char* clientIpAddress)
	: mLogger(sessionCreatorInput.getLogger())
	, mOpenKitConfiguration(sessionCreatorInput.getOpenKitConfiguration())
	, mPrivacyConfiguration(sessionCreatorInput.getPrivacyConfiguration())
	, mContinuousSessionIdProvider(sessionCreatorInput.getSessionIdProvider())
	, mContinuousRandomNumberGenerator(std::make_shared<providers::DefaultPRNGenerator>())
	, mThreadIdProvider(sessionCreatorInput.getThreadIdProvider())
	, mTimingProvider(sessionCreatorInput.getTimingProvider())
	, mBeaconCache(sessionCreatorInput.getBeaconCache())
	, mUseClientIpAddress(clientIpAddress != nullptr)
	, mClientIpAddress(clientIpAddress)
	, mServerId(sessionCreatorInput.getCurrentServerId())
	, mSessionIdProvider(std::make_shared<providers::FixedSessionIDProvider>(mContinuousSessionIdProvider))
	, mRandomNumberGenerator(std::make_shared<providers::FixedPRNGenerator>(mContinuousRandomNumberGenerator))
	, mSessionSequenceNumber(0)
{
}

std::shared_ptr<SessionInternals> SessionCreator::createSession(std::shared_ptr<IOpenKitComposite> parent)
{
	auto configuration = core::configuration::BeaconConfiguration::from(mOpenKitConfiguration, mPrivacyConfiguration, mServerId);
	auto beacon = std::make_shared<protocol::Beacon>(*this, configuration);

	auto session = std::make_shared<Session>(mLogger, parent, beacon);
	mSessionSequenceNumber++;

	return session;
}

void SessionCreator::reset()
{
	mSessionSequenceNumber = 0;
	mSessionIdProvider = std::make_shared<providers::FixedSessionIDProvider>(mContinuousSessionIdProvider);
	mRandomNumberGenerator = std::make_shared<providers::FixedPRNGenerator>(mContinuousRandomNumberGenerator);
}

std::shared_ptr<openkit::ILogger> SessionCreator::getLogger() const
{
	return mLogger;
}

std::shared_ptr<core::caching::IBeaconCache> SessionCreator::getBeaconCache() const
{
	return mBeaconCache;
}

bool SessionCreator::useClientIpAddress() const
{
	return mUseClientIpAddress;
}

const core::UTF8String& SessionCreator::getClientIpAddress() const
{
	return mClientIpAddress;
}

std::shared_ptr<providers::ISessionIDProvider> SessionCreator::getSessionIdProvider() const
{
	return mSessionIdProvider;
}

std::shared_ptr<providers::IThreadIDProvider> SessionCreator::getThreadIdProvider() const
{
	return mThreadIdProvider;
}

std::shared_ptr<providers::ITimingProvider> SessionCreator::getTiminigProvider() const
{
	return mTimingProvider;
}

std::shared_ptr<providers::IPRNGenerator> SessionCreator::getRandomNumberGenerator() const
{
	return mRandomNumberGenerator;
}

int32_t SessionCreator::getSessionSequenceNumber() const
{
	return mSessionSequenceNumber;
}
