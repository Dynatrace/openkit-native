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

#ifndef _CORE_NULLROOTACTION_H
#define _CORE_NULLROOTACTION_H

#include "api/IRootAction.h"
#include "NullAction.h"

#include <memory>

namespace core
{

	///
	/// This class is returned as RootAction by {@ref OpenKit#createSession(String)} when the {@link OpenKit#shutdown()}
	/// has been called before.
	///
	class NullRootAction  : public api::IRootAction, public std::enable_shared_from_this<core::NullRootAction>
	{
	public:

		virtual std::shared_ptr<api::IAction> enterAction(const char* /*actionName*/) override
		{
			return std::shared_ptr<NullAction>(new NullAction(shared_from_this()));
		}
		virtual void leaveAction() override
		{
			// intentionally left empty, due to NullObject pattern
		}

		virtual bool isNullObject() override
		{
			return  true;
		}
	};

}

#endif