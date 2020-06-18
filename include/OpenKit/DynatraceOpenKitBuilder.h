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

#ifndef _OPENKIT_DYNATRACEOPENKITBUILDER_H
#define _OPENKIT_DYNATRACEOPENKITBUILDER_H

#include "OpenKit_export.h"
#include "OpenKit/AbstractOpenKitBuilder.h"

#include <cstdint>

namespace openkit
{
	///
	/// Builder for OpenKit instances connected to Dynatrace
	///
	class OPENKIT_EXPORT DynatraceOpenKitBuilder : public AbstractOpenKitBuilder
	{
	public:

		static constexpr int32_t DEFAULT_SERVER_ID = 1;

		static const std::string OPENKIT_TYPE;

		///
		/// Constructor
		/// @param[in] endpointURL endpoint OpenKit connects to
		/// @param[in] applicationID unique application id
		/// @param[in] deviceID unique device id
		///
		DynatraceOpenKitBuilder(const char* endpointURL, const char* applicationID, int64_t deviceID);

		///
		/// Constructor
		/// @param[in] endpointURL endpoint OpenKit connects to
		/// @param[in] applicationID unique application id
		/// @param[in] deviceID unique device id
		/// @deprecated use DynatraceOpenKitBuilder(const char*, const char*, int64_t) constructor instead
		///
		OPENKIT_DEPRECATED
		DynatraceOpenKitBuilder(const char* endpointURL, const char* applicationID, const char* deviceID);

		///
		/// Destructor
		///
		~DynatraceOpenKitBuilder() override = default;

		///
		/// Sets the application name. The value is only set if it is not @c nullptr.
		/// @param[in] applicationName name of the application
		/// @returns @c this instance
		/// @deprecated This value is set in Dynatrace when creating a Custom application.
		///
		OPENKIT_DEPRECATED
		DynatraceOpenKitBuilder& withApplicationName(const char* applicationName);

		const std::string& getOpenKitType() const override;

		const std::string& getApplicationID() const override;

		const std::string& getApplicationName() const override;

		int32_t getDefaultServerID() const override;

	private:
		/// application id
		std::string mApplicationID;

		/// application name
		std::string mApplicationName;
	};
}

#endif
