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

#ifndef _API_IACTION_H
#define _API_IACTION_H

#include "api/IRootAction.h"
#include <stdint.h>
#include <memory>

namespace api
{
	///
	/// This interface provides functionality to report events/values/errors and traces web requests.
	///
	class IAction
	{
	public:
		///
		/// Destructor
		///
		virtual ~IAction() {}

		///
		/// Leaves this Action.
		/// @returns the parent Action, or @c null if there is no parent Action
		///
		virtual std::shared_ptr<IRootAction> leaveAction() = 0;
	};
}

#endif