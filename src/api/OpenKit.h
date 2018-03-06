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

#ifndef _API_OPENKIT_H
#define _API_OPENKIT_H

#include "OpenKit_export.h"

#ifdef __cplusplus  
extern "C" {
#endif

namespace api {

	class OpenKit
	{
	public:
		///
		/// Constructor
		///
		OpenKit();

		///
		/// Destructor
		///
		~OpenKit() {};

		///
		/// Shuts down OpenKit, ending all open Sessions and waiting for them to be sent.
		///
		OPENKIT_EXPORT void shutdown();
	};
}

#ifdef __cplusplus  
}
#endif 

#endif
