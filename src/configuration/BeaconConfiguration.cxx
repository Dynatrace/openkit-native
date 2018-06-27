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

#include "configuration/BeaconConfiguration.h"

using namespace configuration;

const openkit::DataCollectionLevel BeaconConfiguration::DEFAULT_DATA_COLLECTION_LEVEL = openkit::DataCollectionLevel::USER_BEHAVIOR;
const openkit::CrashReportingLevel BeaconConfiguration::DEFAULT_CRASH_REPORTING_LEVEL = openkit::CrashReportingLevel::OPT_IN_CRASHES;

BeaconConfiguration::BeaconConfiguration()
	: BeaconConfiguration(DEFAULT_DATA_COLLECTION_LEVEL, DEFAULT_CRASH_REPORTING_LEVEL)
{

}

BeaconConfiguration::BeaconConfiguration(openkit::DataCollectionLevel dataLevel, openkit::CrashReportingLevel crashLevel)
	: mDataCollectionLevel(dataLevel)
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