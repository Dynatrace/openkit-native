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

#ifndef _CORE_OBJECTS_OPENKITCOMPOSITE_H
#define _CORE_OBJECTS_OPENKITCOMPOSITE_H

#include "IOpenKitComposite.h"

#include <list>
#include <memory>

namespace core
{
	namespace  objects
	{
		///
		/// A composite base class for OpenKit objects.
		///
		/// @par
		/// It features a container to store child objects.
		/// The container is not thread safe, thus, synchronization must be taken care of by the implementing class.
		///
		class OpenKitComposite
			: public IOpenKitComposite
		{

		public: // functions

			///
			/// Default constructor
			///
			OpenKitComposite()
				: mChildren()
			{
			}

			///
			/// Destructor
			///
			virtual ~OpenKitComposite() {}

			///
			/// Adds the given child object to the list of children.
			///
			/// @param[in] childObject the child object to add.
			///
			void storeChildInList(std::shared_ptr<IOpenKitObject> childObject) override;

			///
			/// Removes the given child object from the list of children.
			///
			/// @param[in] childObject the child object to remove.
			///
			void removeChildFromList(std::shared_ptr<IOpenKitObject> childObject) override;

			///
			/// Returns a shallow copy of the @ref IOpenKitObject child objects
			///
			std::list<std::shared_ptr<IOpenKitObject>> getCopyOfChildObjects() override;

			///
			/// Abstract method to notify the composite about closing/ending of a child object.
			///
			/// @par
			/// The implementing class is fully responsible to handle the implementation.
			/// In most cases removing the child from the container @ref RemoveChildFromList is sufficient.
			///
			/// @param[in] childObject the child object which was closed.
			///
			virtual void onChildClosed(std::shared_ptr<IOpenKitObject> childObject) = 0;

			///
			/// Returns the action ID of this composite or @c 0 if the composite is not an action.
			///
			/// @par
			/// The default implementation returns @c 0.
			/// Action related composites need to override this method and return the appropriate value.
			///
			virtual int32_t getActionId() const;

			virtual void close() = 0;

		private: // members

			///
			/// Default action ID (which is returned by @ref getActionId if not explicitly overridden)
			///
			static constexpr int32_t DEFAULT_ACTION_ID = 0;

			///
			/// Container for storing the children of this composite.
			///
			std::list<std::shared_ptr<IOpenKitObject>> mChildren;
		};

	}
}
#endif
