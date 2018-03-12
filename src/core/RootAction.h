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

#ifndef _CORE_ROOTACTION_H
#define _CORE_ROOTACTION_H

#include "api/IRootAction.h"
#include "Action.h"
#include "protocol/Beacon.h"
#include "UTF8String.h"

#include "memory"

namespace core
{
	///
	/// Actual implementation of the IRootAction interface.
	///
	class RootAction : public api::IRootAction, public Action
	{
	public:

		///
		/// Create a RootAction given a beacon  and the action name
		/// @param[in] beacon the beacon used to serialize this Action
		/// @param[in] name the name of the action
		/// @param[in] parentActions parent actions
		///
		RootAction(std::shared_ptr<protocol::Beacon> beacon, const UTF8String& name, util::SynchronizedQueue<std::shared_ptr<Action>>& parentActions);

		///
		/// Destructor
		///
		virtual  ~RootAction();

		///
		/// Enters an Action with a specified name in this Session.
		/// @param[in] actionName name of the Action
		/// @returns Action instance to work with
		///
		virtual std::shared_ptr<api::IAction> enterAction();


		///
		/// Leaves this action if no leaveAction was already called
		/// Call @c doLeaveAction if this is the first call to @c leaveAction
		/// @returns the parent Action, or @c null if there is no parent Action
		///
		virtual std::shared_ptr<api::IAction> leaveAction() override;

	private:

		///
		/// Leaves this Action.
		/// Called by leaveAction only if this is the first leaveAction call on this Action
		/// @returns the parent Action, or @c null if there is no parent Action
		///
		virtual std::shared_ptr<api::IAction> doLeaveAction();

		/// beacon used for serialization
		std::shared_ptr<protocol::Beacon> mBeacon;

		/// open Actions of children
		util::SynchronizedQueue<std::shared_ptr<Action>> mOpenChildActions;
	};
}
#endif