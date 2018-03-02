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

#ifndef _CORE_ACTION_H
#define _CORE_ACTION_H

#include "api/IAction.h"

#include "memory"

namespace core
{
	///
	/// Actual implementation of the IAction interface.
	///
	class Action : public api::IAction
	{
	public:
		///
		/// Destructor
		///
		virtual ~Action() {};

		///
		/// Leaves this Action.
		/// @returns the parent Action, or @c null if there is no parent Action
		///
		virtual std::shared_ptr<api::IAction> LeaveAction();
	};
}
#endif