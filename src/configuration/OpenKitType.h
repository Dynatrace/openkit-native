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

#ifndef _CONFIGURATION_OPENKITTYPE_H
#define _CONFIGURATION_OPENKITTYPE_H

#include <stdint.h>

namespace configuration
{
	///
	/// This enum defines if an OpenKit instance should be used for AppMon or Dynatrace
	///
	class OpenKitType
	{
	public:
		enum class Type
		{
			APPMON,
			DYNATRACE
		};

		///
		/// Constructor
		/// @param[in] type the OpenKit type, either AppMon or Dynatrace
		///
		OpenKitType(Type type)
			: mType(type)
			, mDefaultServerID(type == Type::APPMON ? APPMON_DEFAULT_SERVER : DYNATRACE_DEFAULT_SERVER)
		{
		}

		///
		/// Returns the default server ID
		/// @returns default server id
		///
		int32_t getDefaultServerID() const;

		///
		/// Returns the OpenKit type as string
		/// @returns the OpenKit type as string
		///
		const char* getTypeString() const;

	private:
		/// The OpenKit type
		Type mType;

		/// default server ID
		int32_t mDefaultServerID;

		static const int32_t APPMON_DEFAULT_SERVER = 1;
		static const int32_t DYNATRACE_DEFAULT_SERVER = 1;
	};	
}

#endif
