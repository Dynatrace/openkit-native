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

#ifndef _CORE_OBJECTS_ACTION_H
#define _CORE_OBJECTS_ACTION_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/IAction.h"
#include "OpenKit/ILogger.h"
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
		/// It is intentional that Action does not serve as a base class for RootAction. This would result in the diamond-inheritance
		/// problem in RootAction. This is because RootAction would inherit from Action which inherits from IAction. But RootAction iself also
		/// inherited from IAction. The code duplication between Action and RootAction is the easiest way to avoid the diamond-inheritance.
		///
		class Action : public openkit::IAction, public std::enable_shared_from_this<Action>
		{
		public:

			///
			/// Create an action given a beacon and the action name
			/// @param[in] logger to write traces to
			/// @param[in] beacon the beacon used to serialize this Action
			/// @param[in] name the name of the action
			///
			Action(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<protocol::Beacon> beacon,
				const UTF8String& name
			);

			///
			/// Create an action given a beacon and the action name
			/// @param[in] logger to write traces to
			/// @param[in] beacon the beacon used to serialize this Action
			/// @param[in] name the name of the action
			/// @param[in] parentAction parent action
			///
			Action(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<protocol::Beacon> beacon,
				const UTF8String& name,
				std::shared_ptr<RootAction> parentAction
			);

			///
			/// Destructor
			///
			virtual ~Action() {}

			std::shared_ptr<IAction> reportEvent(const char* eventName) override;

			std::shared_ptr<IAction> reportValue(const char* valueName, int32_t value) override;

			std::shared_ptr<IAction> reportValue(const char* valueName, double value) override;

			std::shared_ptr<IAction> reportValue(const char* valueName, const char* value) override;

			std::shared_ptr<IAction> reportError(const char* errorName, int32_t errorCode, const char* reason) override;

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url) override;

			virtual std::shared_ptr<openkit::IRootAction> leaveAction() override;

			///
			/// Returns the action ID
			/// @returns the action ID
			///
			virtual int32_t getID() const;

			///
			/// Returns the action name
			/// @returns the action name
			///
			const core::UTF8String& getName() const;

			///
			/// Returns the ID of the parent action
			/// @returns the ID of the parent action
			///
			int32_t getParentID() const;

			///
			/// Returns the start time of the action
			/// @returns the start time of the action
			///
			int64_t getStartTime() const;

			///
			/// Returns the end time of the action
			/// @returns the end time of the action
			///
			int64_t getEndTime() const;

			///
			/// Returns the start sequence number of the action
			/// @returns the start sequence number of the action
			///
			int32_t getStartSequenceNo() const;

			///
			/// Returns the end sequence number of the action
			/// @returns the end sequence number of the action
			///
			int32_t getEndSequenceNo() const;

			///
			/// Return a flag if this action has been closed already
			/// @returns @c true if action was already left, @c false if action is open
			///
			bool isActionLeft() const;

		private:
			///
			/// Leaves this Action.
			/// Called by leaveAction only if this is the first leaveAction call on this Action
			/// @returns the parent Action, or @c nullptr if there is no parent Action
			///
			virtual std::shared_ptr<openkit::IRootAction> doLeaveAction();

			///
			/// Returns a string describing the object, based on some important fields.
			/// This function is indended for debug printouts.
			/// @return a string describing the object
			///
			const std::string toString() const;

			/// Logger to write traces to
			std::shared_ptr<openkit::ILogger> mLogger;

			/// parent action
			std::shared_ptr<RootAction> mParentAction;

			/// action end time
			std::atomic<int64_t> mEndTime;

			/// beacon used for serialization
			std::shared_ptr<protocol::Beacon> mBeacon;

			/// action id
			int32_t mID;

			/// action name
			const core::UTF8String mName;

			/// action start time
			int64_t mStartTime;

			/// action start sequence number
			int32_t mStartSequenceNumber;

			/// action end sequence number
			int32_t mEndSequenceNumber;

			/// Impl object with the actual implementations for Action/RootAction
			ActionCommonImpl mActionImpl;
		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#endif

#endif
