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

#ifndef _CORE_OBJECTS_ISUPPLEMENTARYBASICDATA_H
#define _CORE_OBJECTS_ISUPPLEMENTARYBASICDATA_H

#include <OpenKit/ConnectionType.h>
#include <core/UTF8String.h>

namespace core
{
	namespace objects
	{
		///
		/// Specifies supplementary basic data which will be written to the Beacon
		///
		class ISupplementaryBasicData
		{
		public:
			///
			/// Destructor
			///
			virtual ~ISupplementaryBasicData() = default;

			///
			/// Sets the carrier used by the device
			///
			/// @param[in] carrier carrier
			/// 
			virtual void setCarrier(const core::UTF8String& carrier) = 0;

			///
			/// Returns the carrier used by the device
			///
			/// @return carrier
			/// 
			virtual const core::UTF8String& getCarrier() = 0;

			///
			/// Returns boolean which indicates if the carrier is available
			///
			/// @return state of carrier
			/// 
			virtual bool isCarrierAvailable() = 0;

			///
			/// Resets the isAvailable flag of the carrier
			///
			virtual void resetCarrier() = 0;

			///
			/// Sets the network technology used by the device
			///
			/// @param[in] technology network technology
			/// 
			virtual void setNetworkTechnology(const core::UTF8String& technology) = 0;

			///
			/// Returns the network technology used by the device
			///
			/// @return network technology
			/// 
			virtual const core::UTF8String& getNetworkTechnology() = 0;

			///
			/// Returns boolean which indicates if the network technology is available
			///
			/// @return state of network technology
			/// 
			virtual bool isNetworkTechnologyAvailable() = 0;

			///
			/// Resets the isAvailable flag of the network technology
			///
			virtual void resetNetworkTechnology() = 0;

			///
			/// Sets the connection type used by the device
			///
			/// @param[in] connectionType connection type
			/// 
			virtual void setConnectionType(const openkit::ConnectionType connectionType) = 0;

			///
			/// Returns the connection type used by the device
			///
			/// @return connection type
			/// 
			virtual openkit::ConnectionType getConnectionType() = 0;

			///
			/// Returns boolean which indicates if the connection type is available
			///
			/// @return state of connection type
			/// 
			virtual bool isConnectionTypeAvailable() = 0;

			///
			/// Resets the isAvailable flag of the connection type
			///
			virtual void resetConnectionType() = 0;
		};
	}
}

#endif
