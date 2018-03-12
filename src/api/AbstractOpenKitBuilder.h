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

#ifndef _API_ABSTRACTOPENKITBUILDER_H
#define _API_ABSTRACTOPENKITBUILDER_H

#include "OpenKit.h"

#include <memory>
#include <stdexcept>

namespace api
{
	class OPENKIT_EXPORT AbstractOpenKitBuilder
	{
		public:
			///
			/// Constructor
			/// @param[in] endpointURL endpoint OpenKit connects to
			/// @param[in] deviceID unique device id
			///
			AbstractOpenKitBuilder(const char* endpointURL, uint64_t deviceID);

			///
			/// Destructor
			///
			virtual ~AbstractOpenKitBuilder() {}

			///
			/// Builds an @c OpenKit instance
			/// @returns an @c OpenKit instance
			///
			virtual std::shared_ptr<OpenKit> build() = 0;
	};
}

#endif
