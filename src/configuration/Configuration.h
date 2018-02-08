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

#ifndef _CONFIGURATION_CONFIGURATION_H
#define _CONFIGURATION_CONFIGURATION_H

#include "configuration/HTTPClientConfiguration.h"

namespace configuration {
	///
	/// The Configuration class holds all configuration settings, both provided by the user and the Dynatrace/AppMon server.
	///
	class Configuration
	{
	public:
		///
		/// Construct a Configuration given a @s HTTPClientConfiguration
		/// @param[in] httpClientConfiguration the @s HTTPClientConfiguration to use, will be stored in the @s Configuration
		///
		Configuration(const HTTPClientConfiguration& httpClientConfiguration);

		///
		/// Return the HTTPClientConfiguration to use when constructing a HTTPClient
		/// @returns the @s HTTPClientConfiguration stored in the @s Configuration
		///
		const HTTPClientConfiguration& getHTTPClientConfiguration();

	private:
		const HTTPClientConfiguration& mHTTPClientConfiguration;
	};
}

#endif