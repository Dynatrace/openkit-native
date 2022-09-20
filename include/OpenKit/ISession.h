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

#ifndef _OPENKIT_ISESSION_H
#define _OPENKIT_ISESSION_H

#include "OpenKit/OpenKitExports.h"
#include <OpenKit/json/JsonObjectValue.h>

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace openkit
{
	class OPENKIT_EXPORT IRootAction;
	class OPENKIT_EXPORT IWebRequestTracer;

	///
	/// This interface provides functionality to create Actions in a Session.
	///
	class OPENKIT_EXPORT ISession
	{
	public:
		///
		/// Destructor
		///
		virtual ~ISession() = default;

		///
		/// Enters an Action with a specified name in this Session.
		/// @param[in] actionName name of the Action
		/// @returns Action instance to work with
		///
		virtual std::shared_ptr<IRootAction> enterAction(const char* actionName) = 0;

		///
		/// Tags a session with the provided @c userTag.
		/// If the given @c userTag is @c nullptr or an empty string,
		/// this is equivalent to logging off the user.
		/// 
		/// @par
		/// The last non-empty {@code userTag} is re-applied to split sessions.
		/// Details are described in
		/// https://github.com/Dynatrace/openkit-native/blob/main/docs/internals.md#identify-users-on-split-sessions.
		/// 
		/// @param[in] userTag id of the user
		///
		virtual void identifyUser(const char* userTag) = 0;

		///
		/// Reports a crash with a specified error name, crash reason and a stacktrace.
		///
		/// @par
		/// If given @c errorName is @c nullptr or an empty string, no crash is reported.
		/// If the @c reason is longer than 1000 characters, it is truncated to this value.
		/// If the @c stacktrace is longer than 128.000 characters, it is truncated according to the last line break.
		///
		/// @param[in] errorName  name of the error leading to the crash(e.g.Exception class)
		/// @param[in] reason     reason or description of that error
		/// @param[in] stacktrace stacktrace leading to that crash
		///
		virtual void reportCrash(const char* errorName, const char* reason, const char* stacktrace) = 0;

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
		/// Ends this Session and marks it as ready for immediate sending.
		/// @remarks All previously added action are implicitly closed
		///
		virtual void end() = 0;

		///
		/// Reports a biz event with a mandatory type and additional attributes
		/// @param type type of the event which is mandatory
		/// @param attributes additional attributes (nullptr allowed if none) which are passed along side our internal attributes
		///
		virtual void sendBizEvent(const char* type, const json::JsonObjectValue::JsonObjectMapPtr attributes = nullptr) = 0;

		///
		/// Reports a biz event with a mandatory type and additional attributes
		/// @param name name of the event which is mandatory
		/// @param attributes additional attributes (nullptr allowed if none) which are passed along side our internal attributes
		///
		virtual void sendEvent(const char* name, const json::JsonObjectValue::JsonObjectMapPtr attributes = nullptr) = 0;

	};
}
#endif