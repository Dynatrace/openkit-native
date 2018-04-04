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

#ifndef _CORE_NULLACTION_H
#define _CORE_NULLACTION_H

#include "api/IAction.h"
#include "api/IRootAction.h"

namespace core
{

	///
	/// This class is returned as Action by {@ref OpenKit#createSession(String)} when the {@link OpenKit#shutdown()}
	/// has been called before.
	///
	class NullAction : public api::IAction, public std::enable_shared_from_this<core::NullAction>
	{
	public:

		NullAction()
			: mParentAction(nullptr)
		{}

		NullAction(std::shared_ptr<api::IRootAction> parent)
			: mParentAction(parent)
		{}

		virtual std::shared_ptr<api::IRootAction> leaveAction() override
		{
			return mParentAction;
		}

		virtual bool isNullObject() override
		{
			return  true;
		}

		std::shared_ptr<api::IRootAction> mParentAction;
	};
}

#endif