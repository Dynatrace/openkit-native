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

#ifndef _CORE_NULLSESSION_H
#define _CORE_NULLSESSION_H

#include "api/ISession.h"
#include "core/NullRootAction.h"
#include <memory>

namespace core
{

	///
	/// This class is returned as Session by @ref OpenKit#createSession(String) when the @ref OpenKit#shutdown()
	/// has been called before.
	///
	class NullSession : public api::ISession
	{
	public:

		///
		/// Constructor
		///
		NullSession()
		{

		}

		virtual std::shared_ptr<api::IRootAction> enterAction(const char* /*actionName*/) override
		{
			return std::make_shared<NullRootAction>();
		}

		virtual void identifyUser(const char* /*userTag*/) override
		{
			// intentionally left empty, due to NullObject pattern
		}

		virtual void reportCrash(const char* /*errorName*/, const char* /*reason*/, const char* /*stacktrace*/) override
		{
			// intentionally left empty, due to NullObject pattern
		}

		virtual void end() override
		{
			// intentionally left empty, due to NullObject pattern
		}
	};
}

#endif