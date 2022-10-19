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

#ifndef _CORE_OBJECTS_SUPPLEMENTARYBASICDATA_H
#define _CORE_OBJECTS_SUPPLEMENTARYBASICDATA_H

#include <core/objects/ISupplementaryBasicData.h>
#include <OpenKit/ConnectionType.h>
#include <core/UTF8String.h>

#include <mutex>

namespace core
{
	namespace objects
	{
		///
		/// Specifies supplementary basic data which will be written to the Beacon
		///
		class SupplementaryBasicData : public ISupplementaryBasicData
		{
		public:
			///
			/// Constructor for SupplementaryBasicData
			///
			SupplementaryBasicData();

			///
			/// Destructor
			///
			~SupplementaryBasicData() override = default;

			///
			/// Sets the carrier used by the device
			///
			/// @param[in] carrier carrier
			/// 
			void setCarrier(const core::UTF8String& carrier) override;

			///
			/// Returns the carrier used by the device
			///
			/// @return carrier
			/// 
			const core::UTF8String& getCarrier() override;

			///
			/// Returns boolean which indicates if the carrier is available
			///
			/// @return state of carrier
			/// 
			bool isCarrierAvailable() override;

			///
			/// Resets the isAvailable flag of the carrier
			///
			void resetCarrier() override;

			///
			/// Sets the network technology used by the device
			///
			/// @param[in] technology network technology
			/// 
			void setNetworkTechnology(const core::UTF8String& technology) override;

			///
			/// Returns the network technology used by the device
			///
			/// @return network technology
			/// 
			const core::UTF8String& getNetworkTechnology() override;

			///
			/// Returns boolean which indicates if the network technology is available
			///
			/// @return state of network technology
			/// 
			bool isNetworkTechnologyAvailable() override;

			///
			/// Resets the isAvailable flag of the network technology
			///
			void resetNetworkTechnology() override;

			///
			/// Sets the connection type used by the device
			///
			/// @param[in] connectionType connection type
			/// 
			void setConnectionType(const openkit::ConnectionType connectionType) override;

			///
			/// Returns the connection type used by the device
			///
			/// @return connection type
			/// 
			openkit::ConnectionType getConnectionType() override;

			///
			/// Returns boolean which indicates if the connection type is available
			///
			/// @return state of connection type
			/// 
			bool isConnectionTypeAvailable() override;

			///
			/// Resets the isAvailable flag of the connection type
			///
			void resetConnectionType() override;

		private:
			///
			/// Carrier used by the device
			///
			core::UTF8String mCarrier;

			///
			/// Network technology used by the device
			///
			core::UTF8String mNetworkTechnology;

			///
			/// Connection type used by the device
			///
			openkit::ConnectionType mConnectionType;

			///
			/// Boolean which indicates if the carrier is available
			///
			bool mCarrierAvailable;

			///
			/// Boolean which indicates if the network technology is available
			///
			bool mNetworkTechnologyAvailable;

			///
			/// Boolean which indicates if the connection type is available
			///
			bool mConnectionTypeAvailable;

			///
			/// mutex used for synchronization
			///
			std::mutex mLockObject;
		};
	}
}

#endif
