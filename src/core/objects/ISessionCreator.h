/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _CORE_OBJECTS_ISESSIONCREATOR_H
#define _CORE_OBJECTS_ISESSIONCREATOR_H

#include "IOpenKitComposite.h"
#include "SessionInternals.h"

#include <memory>

namespace core
{
	namespace objects
	{
		class ISessionCreator
		{
		public:

			///
			/// Destructor
			///
			virtual ~ISessionCreator() = default;

			///
			/// Returns a newly created SessionInternals
			///
			/// @param parent the parent composite of the session to create.
			///
			virtual std::shared_ptr<SessionInternals> createSession(std::shared_ptr<IOpenKitComposite> parent) = 0;
		};
	}
}

#endif
