/**
 * Copyright 2018-2021 Dynatrace LLC
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

			///
			/// Resets the internal state of this session creator.
			/// 
			/// A reset includes the following:
			/// - resetting the consecutive session sequence number which is increased every time a session is created.
			/// - use a new session ID (which will stay the same for all newly created sessions).
			/// - use a new randomized number (which will stay the same for all newly created sessions).
			///
			virtual void reset() = 0;
		};
	}
}

#endif
