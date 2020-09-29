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

#include "SessionCreator.h"

#include "core/configuration/BeaconConfiguration.h"
#include "protocol/Beacon.h"
#include "providers/FixedPRNGenerator.h"
#include "providers/FixedSessionIDProvider.h"
#include "providers/DefaultPRNGenerator.h"

using namespace core::objects;

SessionCreator::SessionCreator(ISessionCreatorInput& sessionCreatorInput, const char* clientIpAddress)
	: mLogger(sessionCreatorInput.getLogger())
	, mOpenKitConfiguration(sessionCreatorInput.getOpenKitConfiguration())
	, mPrivacyConfiguration(sessionCreatorInput.getPrivacyConfiguration())
	, mThreadIdProvider(sessionCreatorInput.getThreadIdProvider())
	, mTimingProvider(sessionCreatorInput.getTimingProvider())
	, mBeaconCache(sessionCreatorInput.getBeaconCache())
	, mUseServerSideIpDetermination(clientIpAddress == nullptr)
	, mClientIpAddress(clientIpAddress)
	, mServerId(sessionCreatorInput.getCurrentServerId())
	, mSessionIdProvider(std::make_shared<providers::FixedSessionIDProvider>(sessionCreatorInput.getSessionIdProvider()))
	, mRandomNumberGenerator(std::make_shared<providers::FixedPRNGenerator>(std::make_shared<providers::DefaultPRNGenerator>()))
	, mSessionSequenceNumber(0)
{
}

std::shared_ptr<SessionInternals> SessionCreator::createSession(std::shared_ptr<IOpenKitComposite> parent)
{
	auto configuration = core::configuration::BeaconConfiguration::from(mOpenKitConfiguration, mPrivacyConfiguration, mServerId);
	auto beacon = std::make_shared<protocol::Beacon>(mLogger,
		mBeaconCache,
		configuration,
		mUseServerSideIpDetermination ? nullptr : mClientIpAddress.getStringData().c_str(),
		mSessionIdProvider,
		mThreadIdProvider,
		mTimingProvider,
		mRandomNumberGenerator);

	auto session = std::make_shared<Session>(mLogger, parent, beacon);
	mSessionSequenceNumber++;

	return session;
}

std::shared_ptr<providers::IPRNGenerator> SessionCreator::getRandomNumberGenerator() const
{
	return mRandomNumberGenerator;
}

int32_t SessionCreator::getSessionSequenceNumber() const
{
	return mSessionSequenceNumber;
}
