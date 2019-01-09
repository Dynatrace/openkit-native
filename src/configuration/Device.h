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

#ifndef _CONFIGURATION_DEVICE_H
#define _CONFIGURATION_DEVICE_H

#include "core/UTF8String.h"

namespace configuration
{
	///
	/// Container for device specific information
	///
	class Device
	{
	public:
		///
		/// Constructor
		/// @param[in] operatingSystem name of the operating systeme
		/// @param[in] manufacturer name of the manufacturer
		/// @param[in] modelID model ID
		///
		Device(const core::UTF8String& operatingSystem, const core::UTF8String& manufacturer, const core::UTF8String& modelID);

		///
		/// Returns the operating system
		/// @returns the operating system name
		///
		const core::UTF8String& getOperatingSystem() const;

		///
		/// Returns the manufacturer
		/// @returns the manufacturer name
		///
		const core::UTF8String& getManufacturer() const;


		///
		/// Returns the model ID
		/// @returns the model ID
		///
		const core::UTF8String& getModelID() const;

	private:
		/// name of the operating system
		const core::UTF8String mOperatingSystem;

		/// name of the manufacturer
		const core::UTF8String mManufacturer;

		/// model id
		const core::UTF8String mModelID;
	};
}

#endif