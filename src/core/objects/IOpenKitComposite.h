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

#ifndef _CORE_OBJECTS_IOPENKITCOMPOSITE_H
#define _CORE_OBJECTS_IOPENKITCOMPOSITE_H

#include "IOpenKitObject.h"

#include <list>
#include <memory>

namespace core
{
	namespace objects
	{
		class IOpenKitComposite
			: public IOpenKitObject
		{
		public:

			using ChildList = std::list<std::shared_ptr<IOpenKitObject>>;

			///
			/// Destructor
			///
			~IOpenKitComposite() override = default;

			///
			/// Adds a child object to the list of children.
			///
			/// @param childObject the child object to add.
			///
			virtual void storeChildInList(std::shared_ptr<IOpenKitObject> childObject) = 0;

			///
			/// Removes the given child object from the list of children.
			///
			/// @param childObject the child object to remove
			///
			virtual void removeChildFromList(std::shared_ptr<IOpenKitObject> childObject) = 0;

			///
			/// Returns a shallow copy of the @ref IOpenKitObject child objects
			///
			virtual ChildList getCopyOfChildObjects() = 0;

			///
			/// Returns the current number of children held by this composite.
			///
			virtual ChildList::size_type getChildCount() = 0;

			///
			/// Abstract method to notify the composite about closing/ending a child object.
			///
			/// @par
			/// The implementing class is fully responsible to handle the implementation.
			/// In most cases removing the child from the container (@ref removeChildFromList) is sufficient.
			///
			/// @param childObject the child object which was closed/ended
			///
			virtual void onChildClosed(std::shared_ptr<IOpenKitObject> childObject) = 0;

			///
			/// Returns the action ID of this composite or @c 0 if the composite is not an action.
			///
			virtual int32_t getActionId() const = 0;
		};
	}
}

#endif
