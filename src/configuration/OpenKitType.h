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

#ifndef _CONFIGURATION_OPENKITTYPE_H
#define _CONFIGURATION_OPENKITTYPE_H

namespace configuration
{
	///
	/// This enum defines if an OpenKit instance should be used for AppMon or Dynatrace
	///
	class OpenKitType
	{
	public:
		///
		/// Returns the default server ID
		/// @returns default server id
		///
		int32_t getDefaultServerID() { return mDefaultServerID; }

		/// static instance for AppMon configurations
		static OpenKitType CreateAppMonType() 
		{
			return OpenKitType(appMonDefault);// AppMon: default monitor URL name contains "dynaTraceMonitor" and default Server ID is 1
		}

		/// static instance for Dynatrace configurations
		static OpenKitType CreateDynatraceType()
		{
			return OpenKitType(dynatraceDefault);// Dynatrace: default monitor URL name contains "mbeacon" and default Server ID is 1
		}

	private:

		/// default server ID
		int32_t mDefaultServerID;

		static const int32_t appMonDefault = 1;
		static const int32_t dynatraceDefault = 1;

		///
		/// Constructor
		/// @param[in] defaultServerID default server ID
		///
		OpenKitType(int32_t defaultServerID)
			: mDefaultServerID(defaultServerID)
		{
		}
	};

	
}

#endif
