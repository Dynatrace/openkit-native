/**
 * Copyright 2018-2022 Dynatrace LLC
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

#include <core/objects/SupplementaryBasicData.h>

using namespace core::objects;

SupplementaryBasicData::SupplementaryBasicData()
	: mCarrier()
    , mNetworkTechnology()
	, mConnectionType(openkit::ConnectionType::UNSET)
	, mCarrierAvailable(false)
	, mNetworkTechnologyAvailable(false)
	, mConnectionTypeAvailable(false)
	, mLockObject()
{
}

const core::UTF8String& SupplementaryBasicData::getCarrier()
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		return mCarrier;
	}
}

void SupplementaryBasicData::setCarrier(const core::UTF8String& carrier)
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		mCarrier = carrier;
		mCarrierAvailable = true;
	}
}

bool SupplementaryBasicData::isCarrierAvailable()
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		return mCarrierAvailable;
	}
}

void SupplementaryBasicData::resetCarrier()
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		mCarrierAvailable = false;
	}
}

void SupplementaryBasicData::setNetworkTechnology(const core::UTF8String& technology)
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		mNetworkTechnology = technology;
		mNetworkTechnologyAvailable = true;
	}
}

const core::UTF8String& SupplementaryBasicData::getNetworkTechnology()
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		return mNetworkTechnology;
	}
}

bool SupplementaryBasicData::isNetworkTechnologyAvailable()
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		return mNetworkTechnologyAvailable;
	}
}

void SupplementaryBasicData::resetNetworkTechnology()
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		mNetworkTechnologyAvailable = false;
	}
}

void SupplementaryBasicData::setConnectionType(const openkit::ConnectionType connectionType)
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		mConnectionType = connectionType;
		mConnectionTypeAvailable = true;
	}
}

openkit::ConnectionType SupplementaryBasicData::getConnectionType()
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		return mConnectionType;
	}
}

bool SupplementaryBasicData::isConnectionTypeAvailable()
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		return mConnectionTypeAvailable;
	}
}

void SupplementaryBasicData::resetConnectionType()
{
	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);
		mConnectionTypeAvailable = false;
	}
}