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

#ifndef _CORE_OBJECTS_IACTIONCOMMON_H
#define _CORE_OBJECTS_IACTIONCOMMON_H

#include "OpenKit/IAction.h"
#include "OpenKit/IWebRequestTracer.h"
#include "core/objects/IOpenKitComposite.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/NullAction.h"
#include "core/UTF8String.h"

#include <memory>

namespace core
{
	namespace objects
	{
		///
		/// Defines functionality of a @ref LeafAction respectively a @ref RootAction
		///
		class IActionCommon
		{
		public:
			///
			/// Destructor
			///
			virtual ~IActionCommon() = default;

			///
			/// Enters an action with the given name (only relevant for root actions)
			///
			/// @param actionName the action's name
			/// @return the created / entered action
			virtual std::shared_ptr<openkit::IAction> enterAction
			(
				std::shared_ptr<openkit::IRootAction> rootAction,
				const char* actionName
			) = 0;

			///
			/// Reports a named event (without any value)
			///
			/// @par
			/// If the given @c eventName is @c nullptr then no event is reported to the system
			///
			/// @param eventName the name of the event to report
			/// @returns @c true if this action was not yet closed/left, @c false otherwise.
			///
			virtual void reportEvent(const char* eventName) = 0;

			///
			/// Adds a key-value pair to the beacon.
			///
			/// @param valueName the name of the reported value
			/// @param value  the reported value
			///
			virtual void reportValue(const char* valueName, int32_t value) = 0;

			///
			/// Adds a key-value pair to the beacon.
			///
			/// @param valueName the name of the reported value
			/// @param value  the reported value
			///
			virtual void reportValue(const char* valueName, double value) = 0;

			///
			/// Adds a key-value pair to the beacon.
			///
			/// @param valueName the name of the reported value
			/// @param value  the reported value
			///
			virtual void reportValue(const char* valueName, const char* value) = 0;

			///
			/// Adds an error to the beacon.
			///
			/// @param errorName the name of the reported error.
			/// @param errorCode the reported error code.
			/// @param reason the reason for the error
			///
			virtual void reportError(const char* errorName, int32_t errorCode, const char* reason) = 0;

			///
			/// Adds a web request to the beacon.
			///
			/// @param parent the parent (action or root action) of the web request tracer
			/// @param url the url used by the WebRequestTracer
			///
			virtual std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url) = 0;

			///
			/// Leaves this action.
			///
			/// @returns @c true if this action was not yet left/closed, @c otherwise.
			virtual bool leaveAction() = 0;

			///
			/// Indicates if this action was already closed/left.
			///
			virtual bool isActionLeft() const = 0;

			///
			/// Returns the action's ID
			///
			virtual int32_t getID() const = 0;

			///
			/// Returns the ID of the action's parent
			///
			virtual int32_t getParentID() const = 0;

			///
			/// Returns the action's name
			///
			virtual const core::UTF8String& getName() const = 0;

			///
			/// Returns the action's start time
			///
			virtual int64_t getStartTime() const = 0;

			///
			/// Returns the action's end time
			///
			virtual int64_t getEndTime() const = 0;

			///
			/// Returns the action's start sequence number
			///
			virtual int32_t getStartSequenceNumber() const = 0;

			///
			/// Returns the action's end sequence number
			///
			virtual int32_t getEndSequenceNumber() const = 0;

			virtual void onChildClosed(std::shared_ptr<IOpenKitObject> childObject) = 0;

			///
			/// Returns a string describing this action (including the most important fields).
			///
			/// @par
			/// This function is intended to be used when debugging.
			///
			virtual const std::string toString() const = 0;
		};
	}
}

#endif
