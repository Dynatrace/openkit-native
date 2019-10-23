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

#include "BeaconConfiguration.h"
#include "ConfigurationDefaults.h"

using namespace core::configuration;

BeaconConfiguration::BeaconConfiguration()
	: BeaconConfiguration(DEFAULT_MULTIPLICITY, DEFAULT_DATA_COLLECTION_LEVEL, DEFAULT_CRASH_REPORTING_LEVEL)
{
}

BeaconConfiguration::BeaconConfiguration(int32_t mulitplicity, openkit::DataCollectionLevel dataLevel, openkit::CrashReportingLevel crashLevel)
	: mMultiplicity(mulitplicity)
	, mDataCollectionLevel(dataLevel)
	, mCrashReportingLevel(crashLevel)
{

}

openkit::DataCollectionLevel BeaconConfiguration::getDataCollectionLevel() const
{
	return mDataCollectionLevel;
}

openkit::CrashReportingLevel BeaconConfiguration::getCrashReportingLevel() const
{
	return mCrashReportingLevel;
}

int32_t BeaconConfiguration::getMultiplicity() const
{
	return mMultiplicity;
}
