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

#ifndef _OPENKIT_IACTION_H
#define _OPENKIT_IACTION_H

#include "OpenKit/OpenKitExports.h"

#include <chrono>
#include <cstdint>
#include <memory>

namespace openkit
{
	class OPENKIT_EXPORT IRootAction;
	class OPENKIT_EXPORT IWebRequestTracer;

	///
	/// This interface provides functionality to report events/values/errors and traces web requests.
	///
	class OPENKIT_EXPORT IAction
	{
	public:
		///
		/// Destructor
		///
		virtual ~IAction() {}

		///
		/// Reports an event with a specified name (but without any value).
		///
		/// If given @c eventName is @c nullptr then no event is reported to the system.
		///
		/// @param eventName name of the event
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportEvent(const char* eventName) = 0;

		///
		/// Reports a 32-bit int value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportValue(const char* valueName, int32_t value) = 0;

		///
		/// Reports a 64-bit int value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportValue(const char* valueName, int64_t value) = 0;

		///
		/// Reports a double value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportValue(const char* valueName, double value) = 0;

		///
		/// Reports a String value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportValue(const char* valueName, const char* value) = 0;

		///
		/// Reports an error with a specified name and error code.
		///
		/// @param errorName name of this error
		/// @param errorCode numeric error code of this error
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportError(const char* errorName, int32_t errorCode) = 0;

		///
		/// Reports an error with a specified name and parameters describing the cause of this error.
		///
		/// @par
		/// If given @c errorName is @c nullptr or an empty string, no error is reported.
		/// If the @c causeDescription is longer than 1000 characters, it is truncated to this value.
		/// If the @c causeStackTrace is longer than 128.000 characters, it is truncated according to the last line break.
		///
		/// @param errorName name of this error
		/// @param causeName name describing the cause of the error (e.g. exception class name)
		/// @param causeDescription description what caused the eror (e.g. exception message)
		/// @param causeStackTrace stack trace of the error
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportError(
			const char* errorName,
			const char* causeName,
			const char* causeDescription,
			const char* causeStackTrace
		) = 0;

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
		/// Leaves this IAction.
		///
		/// @returns the parent IRootAction.
		///
		virtual std::shared_ptr<IRootAction> leaveAction() = 0;

		///
		/// Cancels this IAction.
		///
		/// @par
		/// Canceling an IAction is similar to IAction::leaveAction(), except
		/// that the data and all unfinished child objects are discarded
		/// instead of being sent.
		///
		/// @returns the parent IRootAction.
		///
		virtual std::shared_ptr<IRootAction> cancelAction() = 0;

		///
		/// Get the duration of this IAction.
		///
		/// @par
		/// The duration of an IAction is equal to current timestamp - start timestamp
		/// if the action is still open, or end timestamp - start timestamp, if 
		/// IAction::leaveAction() or IAction::cancelAction() was already called.
		///
		/// @return The duration of this IAction in milliseconds
		///
		virtual std::chrono::milliseconds getDuration() = 0;
	};
}

#endif
