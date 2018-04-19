/**
* Copyright 2018 Dynatrace LLC
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

#ifndef _API_ISESSION_H
#define _API_ISESSION_H

#include "OpenKit_export.h"

#include <stdint.h>
#include <memory>

namespace api
{
	class IRootAction;

	///
	/// This interface provides functionality to create Actions in a Session.
	///
	class OPENKIT_EXPORT ISession
	{
	public:
		///
		/// Destructor
		///
		virtual ~ISession() {}

		///
		/// Enters an Action with a specified name in this Session.
		/// @param[in] actionName name of the Action
		/// @returns Action instance to work with
		///
		virtual std::shared_ptr<IRootAction> enterAction(const char* actionName) = 0;

		///
		/// Tags a session with the provided @c userTag.
		/// If the given @c userTag is @c nullptr or an empty string,
		/// no user identification will be reported to the server.
		/// @param[in] userTag id of the user
		///
		virtual void identifyUser(const char* userTag) = 0;

		///
		/// Reports a crash with a specified error name, crash reason and a stacktrace.
		/// @param[in] errorName  name of the error leading to the crash(e.g.Exception class)
		/// @param[in] reason     reason or description of that error
		/// @param[in] stacktrace stacktrace leading to that crash
		///
		virtual void reportCrash(const char* errorName, const char* reason, const char* stacktrace) = 0;

		///
		/// Ends this Session and marks it as ready for immediate sending.
		/// @remarks All previously added action are implicitly closed
		///
		virtual void end() = 0;
	};
}
#endif