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

#ifndef _CORE_OBJECTS_ACTIONCOMMONIMPL_H
#define _CORE_OBJECTS_ACTIONCOMMONIMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/ILogger.h"
#include "OpenKit/IWebRequestTracer.h"
#include "core/UTF8String.h"
#include "core/objects/IActionCommon.h"
#include "core/objects/NullWebRequestTracer.h"
#include "core/objects/OpenKitComposite.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

namespace protocol
{
	class IBeacon;
}

namespace core
{
	namespace objects
	{
		///
		/// Wrapper/PIMPL for functions used by Action and RootAction to avoid code duplication
		///
		class ActionCommonImpl
			: public OpenKitComposite
			, public IActionCommon
			, public std::enable_shared_from_this<ActionCommonImpl>
		{
		public:

			///
			/// Constructor
			/// @param[in] logger logger instance to use
			/// @param[in] beacon for this session that will serialize the data
			/// @param[in] objectID instance details serialization used for logging
			///
			ActionCommonImpl
			(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<protocol::IBeacon> beacon,
				std::shared_ptr<IOpenKitComposite> parent,
				const core::UTF8String& name,
				const std::string& actionClassName
			);

			~ActionCommonImpl() override = default;

			std::shared_ptr<openkit::IAction> enterAction
			(
				std::shared_ptr<openkit::IRootAction>,
				const char* actionName
			) override;

			void reportEvent(const char* eventName) override;

			void reportValue(const char* valueName, int32_t value) override;

			void reportValue(const char* valueName, double value) override;

			void reportValue(const char* valueName, const char* value) override;

			void reportError(const char* errorName, int32_t errorCode, const char* reason) override;

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url) override;

			bool leaveAction() override;

			bool isActionLeft() const override;

			int32_t getID() const override;

			int32_t getParentID() const override;

			const core::UTF8String& getName() const override;

			int64_t getStartTime() const override;

			int64_t getEndTime() const override;

			int32_t getStartSequenceNumber() const override;

			int32_t getEndSequenceNumber() const override;

			int32_t getActionId() const override;

			void onChildClosed(std::shared_ptr<IOpenKitObject> childObject) override;

			void close() override;

			const std::string toString() const override;

		private:
			using Mutex_t = std::mutex;

			/// logger instance
			std::shared_ptr<openkit::ILogger> mLogger;

			/// beacon collection and sending this session's data
			std::shared_ptr<protocol::IBeacon> mBeacon;

			/// the action's parent composite
			std::shared_ptr<IOpenKitComposite> mParent;

			/// action's name
			const core::UTF8String mName;

			/// action's start time
			int64_t mStartTime;

			/// action's end time
			int64_t mEndTime;

			/// action's start sequence number
			int32_t mStartSequenceNumber;

			/// action's end sequence number
			int32_t mEndSequenceNumber;

			/// the action ID
			int32_t mActionID;

			/// the ID of the action's parent
			int32_t mParentID;

			/// object information
			const std::string mActionClassName;

			/// indicates if the action was already closed/left
			std::atomic<bool> mIsActionLeft;

			/// synchronization lock object
			Mutex_t mMutex;
		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif