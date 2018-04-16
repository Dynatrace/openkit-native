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

#ifndef _API_APPMONMOPENKITBUILDER_H
#define _API_APPMONMOPENKITBUILDER_H

#include "AbstractOpenKitBuilder.h"

namespace api
{
	class OPENKIT_EXPORT AppMonOpenKitBuilder : public AbstractOpenKitBuilder
	{
	public:
		///
		/// Constructor
		/// @param[in] endPointURL endpoint OpenKit connects to
		/// @param[in] applicationID unique application id
		/// @param[in] deviceID unique device id
		///
		AppMonOpenKitBuilder(const char* endpointURL, const char* applicationID, uint64_t deviceID);

		///
		/// Destructor
		///
		virtual ~AppMonOpenKitBuilder() {}

	};
}

#endif
