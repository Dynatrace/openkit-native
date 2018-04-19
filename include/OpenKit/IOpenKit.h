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

#ifndef _OPENKIT_IOPENKIT_H
#define _OPENKIT_IOPENKIT_H

#include "OpenKit_export.h"

#include <memory>

namespace openkit
{
	class ILogger;
	class ISession;

	class OPENKIT_EXPORT IOpenKit
	{
	public:
		///
		/// Destructor
		///
		virtual ~IOpenKit() {}

		///
		/// Waits until OpenKit is fully initialized.
		///
		/// The calling thread is blocked until OpenKit is fully initialized or until OpenKit is shut down using the
		/// shutdown() method.
		/// 
		/// Be aware, if @ref AbstractOpenKitBuilder is wrongly configured, for example when creating an
		/// instance with an incorrect endpoint URL, then this method might hang indefinitely, unless shutdown() is called.
		/// @returns @c true when OpenKit is fully initialized, @c false when a shutdown request was made.
		///
		virtual bool waitForInitCompletion() = 0;

		///
		/// Waits until OpenKit is fully initialized or the given timeout expired
		///
		/// The calling thread is blocked until OpenKit is fully initialized or until OpenKit is shut down using the
		/// shutdown() method or the timeout expired.
		/// 
		/// Be aware, if @ref AbstractOpenKitBuilder is wrongly configured, for example when creating an
		/// instance with an incorrect endpoint URL, then this method might hang indefinitely, unless shutdown() is called
		/// or timeout expires.
		/// @param[in] timeoutMillis The maximum number of milliseconds to wait for initialization being completed.
		/// @returns @c true when OpenKit is fully initialized, @c false when a shutdown request was made or @c timeoutMillis expired.
		///
		virtual bool waitForInitCompletion(int64_t timeoutMillis) = 0;

		///
		/// Returns whether OpenKit is initialized or not.
		/// @returns @c true if OpenKit is fully initialized, @c false if OpenKit still performs initialization.
		///
		virtual bool isInitialized() const = 0;

		///
		/// Creates a Session instance which can then be used to create Actions.
		/// @param[in] clientIPAddress client IP address where this Session is coming from
		/// @returns @ref Session instance to work with
		///
		virtual std::shared_ptr<openkit::ISession> createSession(const char* clientIPAddress) = 0;

		///
		/// Shuts down OpenKit, ending all open Sessions and waiting for them to be sent.
		///
		virtual void shutdown() = 0;
	};
}

#endif
