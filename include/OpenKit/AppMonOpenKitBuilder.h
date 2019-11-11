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

#ifndef _OPENKIT_APPMONMOPENKITBUILDER_H
#define _OPENKIT_APPMONMOPENKITBUILDER_H

#include "OpenKit_export.h"
#include "OpenKit/AbstractOpenKitBuilder.h"

#include <cstdint>

namespace openkit
{
	///
	/// Builder for OpenKit instances connected to AppMon
	///
	class OPENKIT_EXPORT AppMonOpenKitBuilder : public AbstractOpenKitBuilder
	{
	public:

		static constexpr int32_t DEFAULT_SERVER_ID = 1;

		static const std::string OPENKIT_TYPE;

		///
		/// Constructor
		/// @param[in] endpointURL endpoint OpenKit connects to
		/// @param[in] applicationName unique application name
		/// @param[in] deviceID unique device id
		///
		AppMonOpenKitBuilder(const char* endpointURL, const char* applicationName, int64_t deviceID);

		///
		/// Constructor
		/// @param[in] endpointURL endpoint OpenKit connects to
		/// @param[in] applicationName unique application name
		/// @param[in] deviceID unique device id
		/// @deprecated use AppMonOpenKitBuilder(const char*, const char*, int64_t) constructor instead
		///
		OPENKIT_DEPRECATED
		AppMonOpenKitBuilder(const char* endpointURL, const char* applicationName, const char* deviceID);

		///
		/// Destructor
		///
		~AppMonOpenKitBuilder() override = default;

		const std::string& getOpenKitType() const override;

		const std::string& getApplicationID() const override;

		const std::string& getApplicationName() const override;

		int32_t getDefaultServerID() const override;

	private:

		/// application name
		std::string mApplicationName;
	};
}

#endif
