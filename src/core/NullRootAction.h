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

#ifndef _CORE_NULLROOTACTION_H
#define _CORE_NULLROOTACTION_H

#include "OpenKit/IRootAction.h"
#include "NullAction.h"
#include "NullWebRequestTracer.h"

#include <memory>

namespace core
{

	///
	/// This class is returned as RootAction by @ref openkit::IOpenKit::createSession(const char*) when the @ref openkit::IOpenKit::shutdown()
	/// has been called before.
	///
	class NullRootAction  : public openkit::IRootAction, public std::enable_shared_from_this<core::NullRootAction>
	{
	public:

		virtual std::shared_ptr<openkit::IAction> enterAction(const char* /*actionName*/) override
		{
			return std::shared_ptr<NullAction>(new NullAction(shared_from_this()));
		}

		virtual std::shared_ptr<IRootAction> reportEvent(const char* /*eventName*/) override
		{
			return shared_from_this();
		}

		virtual std::shared_ptr<IRootAction> reportValue(const char* /*valueName*/, int32_t /*value*/) override
		{
			return shared_from_this();
		}

		virtual std::shared_ptr<IRootAction> reportValue(const char* /*valueName*/, double /*value*/) override
		{
			return shared_from_this();
		}

		virtual std::shared_ptr<IRootAction> reportValue(const char* /*valueName*/, const char* /*value*/) override
		{
			return shared_from_this();
		}

		virtual std::shared_ptr<IRootAction> reportError(const char* /*errorName*/, int32_t /*errorCode*/, const char* /*reason*/) override
		{
			return shared_from_this();
		}

		virtual std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* /*url*/) override
		{
			return std::make_shared<NullWebRequestTracer>();
		}

		virtual void leaveAction() override
		{
			// intentionally left empty, due to NullObject pattern
		}
	};

}

#endif
