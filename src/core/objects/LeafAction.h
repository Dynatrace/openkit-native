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

#ifndef _CORE_OBJECTS_ACTION_H
#define _CORE_OBJECTS_ACTION_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/IAction.h"
#include "OpenKit/ILogger.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/OpenKitComposite.h"
#include "core/util/SynchronizedQueue.h"
#include "core/UTF8String.h"
#include "NullWebRequestTracer.h"
#include "ActionCommonImpl.h"

#include <memory>
#include <atomic>

namespace protocol
{
	class Beacon;
}

namespace core
{
	namespace objects
	{
		class RootAction;
		///
		/// Actual implementation of the IAction interface.
		///
		/// @par
		/// It is intentional that Action does not serve as a base class for RootAction. This would result in the
		/// diamond-inheritance problem in RootAction. This is because RootAction would inherit from Action which
		/// inherits from IAction. But RootAction itself also inherited from IAction.
		/// The code duplication between Action and RootAction is the easiest way to avoid the diamond-inheritance.
		///
		/// @par
		/// This class is just a facade to the actual @ref ActionCommonImpl implementation.
		class LeafAction
			: public openkit::IAction
			, public std::enable_shared_from_this<LeafAction>
		{
		public:

			///
			/// Create an action with the given action implementation and the given parent action.
			///
			/// @param[in] actionImpl the actual action implementation.
			/// @param[in] parentAction parent action
			///
			LeafAction(
				std::shared_ptr<IActionCommon> actionImpl,
				std::shared_ptr<openkit::IRootAction> parentAction
			);

			///
			/// Destructor
			///
			~LeafAction() override = default;

			std::shared_ptr<IAction> reportEvent(const char* eventName) override;

			std::shared_ptr<IAction> reportValue(const char* valueName, int32_t value) override;

			std::shared_ptr<IAction> reportValue(const char* valueName, int64_t value) override;

			std::shared_ptr<IAction> reportValue(const char* valueName, double value) override;

			std::shared_ptr<IAction> reportValue(const char* valueName, const char* value) override;

			std::shared_ptr<IAction> reportError(const char* errorName, int32_t errorCode) override;

			std::shared_ptr<IAction> reportError(
				const char* errorName,
				const char* causeName,
				const char* causeDescription,
				const char* causeStackTrace
			) override;

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url) override;

			std::shared_ptr<openkit::IRootAction> leaveAction() override;

			std::shared_ptr<openkit::IRootAction> cancelAction() override;

			std::chrono::milliseconds getDuration() override;

			///
			/// Returns the actual action implementation.
			///
			/// NOTE: Intended to be used by tests only
			///
			const std::shared_ptr<IActionCommon> getActionImpl();

		private:
			/// Impl object with the actual implementations for Action/RootAction
			const std::shared_ptr<IActionCommon> mActionImpl;

			/// parent action
			const std::shared_ptr<openkit::IRootAction> mParentAction;

		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
