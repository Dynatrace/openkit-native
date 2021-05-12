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

#ifndef _OPENKIT_IROOTACTION_H
#define _OPENKIT_IROOTACTION_H

#include "OpenKitExports.h"

#include <cstdint>
#include <memory>

namespace openkit
{
	class OPENKIT_EXPORT IWebRequestTracer;
	class OPENKIT_EXPORT IAction;

	///
	/// This interface provides the same functionality as IAction, additionally it allows to create child actions
	///
	class OPENKIT_EXPORT IRootAction
	{
	public:

		virtual ~IRootAction() {}

		///
		/// Enters an Action with a specified name in this root action.
		/// @param[in] actionName name of the Action
		/// @returns Action instance to work with
		///
		virtual std::shared_ptr<IAction> enterAction(const char* actionName) = 0;

		///
		/// Reports an event with a specified name (but without any value).
		///
		/// If given @c eventName is @c nullptr then no event is reported to the system.
		///
		/// @param eventName name of the event
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IRootAction> reportEvent(const char* eventName) = 0;

		///
		/// Reports a 32-bit int value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IRootAction> reportValue(const char* valueName, int32_t value) = 0;

		///
		/// Reports a 64-bit int value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IRootAction> reportValue(const char* valueName, int64_t value) = 0;

		///
		/// Reports a double value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IRootAction> reportValue(const char* valueName, double value) = 0;

		///
		/// Reports a String value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IRootAction> reportValue(const char* valueName, const char* value) = 0;

		///
		/// Reports an error with a specified name, error code and reason.
		///
		/// @param errorName name of this error
		/// @param errorCode numeric error code of this error
		/// @param reason    reason for this error
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IRootAction> reportError(const char* errorName, int32_t errorCode, const char* reason) = 0;

		///
		/// Allows tracing and timing of a web request handled by any 3rd party HTTP Client (e.g. CURL, EasyHttp, ...).
		/// In this case the Dynatrace HTTP header (@ref openkit::OpenKitConstants::WEBREQUEST_TAG_HEADER) has to be set manually to the
		/// tag value of this WebRequestTracer. <br>
		/// If the web request is continued on a server-side Agent (e.g. Java, .NET, ...) this Session will be correlated to
		/// the resulting server-side PurePath.
		///
		/// @param url the URL of the web request to be tagged and timed
		/// @return a WebRequestTracer which allows getting the tag value and adding timing information
		///
		virtual std::shared_ptr<IWebRequestTracer> traceWebRequest(const char* url) = 0;

		///
		/// Leaves this Action.
		///
		virtual void leaveAction() = 0;
	};
}
#endif
