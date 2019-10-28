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

#ifndef _CORE_CONFIGURATION_BEACONCONFIGURATION_H
#define _CORE_CONFIGURATION_BEACONCONFIGURATION_H

#include "IBeaconConfiguration.h"

namespace core
{
	namespace configuration
	{
		///
		/// Configuration for beacon cache.
		///
		class BeaconConfiguration
			: public IBeaconConfiguration
		{
		public:
			///
			/// Constructor
			/// @param[in] dataLevel data collection level
			/// @param[in] crashLevel crash reporting level
			///
			BeaconConfiguration(int32_t multiplicity);

			///
			/// Default constructor returning the default config
			///
			BeaconConfiguration();


			///
			/// Get the multiplicity
			/// @return the multiplicity
			///
			int32_t getMultiplicity() const override;

			///
			/// Returns a boolean indicating if capturing is allowed based on the value of multiplicity
			///
			bool isCapturingAllowed() const override;

		private:
			/// multiplicity controlling how many sessions are actually send
			int32_t mMultiplicity;
		};
	}
}

#endif
