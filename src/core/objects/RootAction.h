/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _CORE_OBJECTS_ROOTACTION_H
#define _CORE_OBJECTS_ROOTACTION_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/IRootAction.h"
#include "OpenKit/ILogger.h"
#include "core/UTF8String.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/OpenKitComposite.h"
#include "NullAction.h"
#include "NullWebRequestTracer.h"
#include "ActionCommonImpl.h"

#include <memory>

namespace protocol
{
	class IBeacon;
}

namespace core
{
	namespace objects
	{
		class Session;

		///
		/// Actual implementation of the IRootAction interface.
		///
		/// @par
		/// It is intentional that Action does not serve as a base class for RootAction. This would result in the
		/// diamond-inheritance problem in RootAction. This is because RootAction would inherit from Action which
		/// inherits from IAction. But RootAction itself also inherited from IAction.
		/// The code duplication between Action and RootAction is the easiest way to avoid the diamond-inheritance.
		///
		/// @par
		/// This class is just a facade to the actual @ref ActionCommonImpl implementation.
		///
		class RootAction
			: public openkit::IRootAction
			, public std::enable_shared_from_this<RootAction>
		{
		public:

			///
			/// Create a RootAction given a beacon  and the action name
			///
			/// @param[in] actionImpl the actual action implementation.
			/// @param[in] session the session object keeping track of all root actions of this level
			///
			RootAction(
				std::shared_ptr<IActionCommon> actionImpl
			);

			///
			/// Destructor
			///
			~RootAction() override = default;

			std::shared_ptr<openkit::IAction> enterAction(const char* actionName) override;

			std::shared_ptr<IRootAction> reportEvent(const char* eventName) override;

			std::shared_ptr<IRootAction> reportValue(const char* valueName, int32_t value) override;

			std::shared_ptr<IRootAction> reportValue(const char* valueName, int64_t value) override;

			std::shared_ptr<IRootAction> reportValue(const char* valueName, double value) override;

			std::shared_ptr<IRootAction> reportValue(const char* valueName, const char* value) override;

			std::shared_ptr<IRootAction> reportError(const char* errorName, int32_t errorCode, const char* reason) override;

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url) override;

			void leaveAction() override;

			std::shared_ptr<IActionCommon> getActionImpl();

		private:
			/// Impl object with the actual implementations for Action/RootAction
			std::shared_ptr<IActionCommon> mActionImpl;

		};
	}
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif