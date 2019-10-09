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

#ifndef _CORE_OBJECTS_NULLROOTACTION_H
#define _CORE_OBJECTS_NULLROOTACTION_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/IAction.h"
#include "OpenKit/IRootAction.h"
#include "OpenKit/IWebRequestTracer.h"

#include <memory>

namespace core
{
	namespace objects
	{
		///
		/// This class is returned as RootAction by @ref openkit::IOpenKit::createSession(const char*) when the
		/// @ref openkit::IOpenKit::shutdown() has been called before.
		///
		class NullRootAction
			: public openkit::IRootAction
			, public std::enable_shared_from_this<NullRootAction>
		{
		public:

			static const std::shared_ptr<NullRootAction> INSTANCE;

			std::shared_ptr<openkit::IAction> enterAction(const char* /*actionName*/) override;

			std::shared_ptr<openkit::IRootAction> reportEvent(const char* /*eventName*/) override;

			std::shared_ptr<openkit::IRootAction> reportValue(const char* /*valueName*/, int32_t /*value*/) override;

			std::shared_ptr<openkit::IRootAction> reportValue(const char* /*valueName*/, double /*value*/) override;

			std::shared_ptr<openkit::IRootAction> reportValue(const char* /*valueName*/, const char* /*value*/) override;

			std::shared_ptr<openkit::IRootAction> reportError(const char* /*errorName*/, int32_t /*errorCode*/, const char* /*reason*/) override;

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* /*url*/) override;

			void leaveAction() override;
		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
