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

#ifndef _CORE_OBJECTS_NULLACTION_H
#define _CORE_OBJECTS_NULLACTION_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/IAction.h"
#include "OpenKit/IRootAction.h"
#include "NullWebRequestTracer.h"

namespace core
{
	namespace objects
	{
		///
		/// This class is returned as Action by @ref openkit::IOpenKit::createSession(const char*) when the @ref openkit::IOpenKit::shutdown()
		/// has been called before.
		///
		class NullAction : public openkit::IAction, public std::enable_shared_from_this<NullAction>
		{
		public:

			NullAction()
				: NullAction(nullptr)
			{}

			NullAction(std::shared_ptr<openkit::IRootAction> parent)
				: mParentAction(parent)
			{}

			std::shared_ptr<IAction> reportEvent(const char* /*eventName*/) override
			{
				return shared_from_this();
			}

			std::shared_ptr<IAction> reportValue(const char* /*valueName*/, int32_t /*value*/) override
			{
				return shared_from_this();
			}

			std::shared_ptr<IAction> reportValue(const char* /*valueName*/, int64_t /*value*/) override
			{
				return shared_from_this();
			}

			std::shared_ptr<IAction> reportValue(const char* /*valueName*/, double /*value*/) override
			{
				return shared_from_this();
			}

			std::shared_ptr<IAction> reportValue(const char* /*valueName*/, const char* /*value*/) override
			{
				return shared_from_this();
			}

			std::shared_ptr<IAction> reportError(const char* /*errorName*/, int32_t /*errorCode*/) override
			{
				return shared_from_this();
			}

			std::shared_ptr<IAction> reportError(
				const char* /*errorName*/,
				const char* /*causeName*/,
				const char* /*causeDescription*/,
				const char* /*causeStackTrace*/
			) override
			{
				return shared_from_this();
			}

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* /*url*/) override
			{
				return NullWebRequestTracer::instance();
			}

			std::shared_ptr<openkit::IRootAction> leaveAction() override
			{
				return mParentAction;
			}

			std::shared_ptr<openkit::IRootAction> cancelAction() override
			{
				return mParentAction;
			}

			std::chrono::milliseconds getDuration() override
			{
				return std::chrono::milliseconds(0);
			}

		private:

			std::shared_ptr<openkit::IRootAction> mParentAction;
		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
