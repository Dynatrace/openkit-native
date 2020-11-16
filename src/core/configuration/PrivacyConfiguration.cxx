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

#include "PrivacyConfiguration.h"
#include "ConfigurationDefaults.h"

using namespace core::configuration;

PrivacyConfiguration::PrivacyConfiguration(
	const openkit::IOpenKitBuilder& builder
)
	: mDataCollectionLevel(builder.getDataCollectionLevel())
	, mCrashReportingLevel(builder.getCrashReportingLevel())
{
}

std::shared_ptr<IPrivacyConfiguration> PrivacyConfiguration::from(const openkit::IOpenKitBuilder& builder)
{
	return std::make_shared<PrivacyConfiguration>(builder);
}

openkit::DataCollectionLevel PrivacyConfiguration::getDataCollectionLevel() const
{
	return mDataCollectionLevel;
}

openkit::CrashReportingLevel PrivacyConfiguration::getCrashReportingLevel() const
{
	return mCrashReportingLevel;
}

bool PrivacyConfiguration::isDeviceIdSendingAllowed() const
{
	return mDataCollectionLevel == openkit::DataCollectionLevel::USER_BEHAVIOR;
}

bool PrivacyConfiguration::isSessionNumberReportingAllowed() const
{
	return mDataCollectionLevel == openkit::DataCollectionLevel::USER_BEHAVIOR;
}

bool PrivacyConfiguration::isWebRequestTracingAllowed() const
{
	return mDataCollectionLevel != openkit::DataCollectionLevel::OFF;
}

bool PrivacyConfiguration::isSessionReportingAllowed() const
{
	return mDataCollectionLevel != openkit::DataCollectionLevel::OFF;
}

bool PrivacyConfiguration::isActionReportingAllowed() const
{
	return mDataCollectionLevel != openkit::DataCollectionLevel::OFF;
}

bool PrivacyConfiguration::isValueReportingAllowed() const
{
	return mDataCollectionLevel == openkit::DataCollectionLevel::USER_BEHAVIOR;
}

bool PrivacyConfiguration::isEventReportingAllowed() const
{
	return mDataCollectionLevel == openkit::DataCollectionLevel::USER_BEHAVIOR;
}

bool PrivacyConfiguration::isErrorReportingAllowed() const
{
	return mDataCollectionLevel != openkit::DataCollectionLevel::OFF;
}

bool PrivacyConfiguration::isCrashReportingAllowed() const
{
	return mCrashReportingLevel == openkit::CrashReportingLevel::OPT_IN_CRASHES;
}

bool PrivacyConfiguration::isUserIdentificationAllowed() const
{
	return mDataCollectionLevel == openkit::DataCollectionLevel::USER_BEHAVIOR;
}