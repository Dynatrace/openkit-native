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


#ifndef _CORE_ISESSIONWATCHDOGCONTEXT_H
#define _CORE_ISESSIONWATCHDOGCONTEXT_H

#include "core/objects/ISessionProxy.h"
#include "core/objects/SessionInternals.h"

#include <memory>
#include <cstdint>

namespace core
{
	///
	/// A context keeping track of:
	///   - SessionInternals which could not be finished after session splitting and which will
    ///         forcefully finished after a grace period.
	///   - ISessionProxy which need to be split after maximum session duration / idle timeout
	///
	class ISessionWatchdogContext
	{
	public:

		///
		/// Destructor
		///
		virtual ~ISessionWatchdogContext() = default;

		///
		/// Looks over the tracked sessions / session proxies and finishes / splits the ones where the respective time
		/// is expired.
		///
		virtual void execute() = 0;

		///
		/// Requests this context to finish up and shutdown.
		///
		virtual void requestShutdown() = 0;

		///
		/// Indicates whether shutdown was requested on this context or not.
		///
		/// @return @c true if shutdown was requested, @c false otherwise
		///
		virtual bool isShutdownRequested() = 0;

		///
		/// Tries to close/end the given session or enqueues it for closing after the given grace period.
		///
		/// @param session the session to be closed or enqueued for closing.
		/// @param closeGracePeriodInMillis the grace period after which the session is to be forcefully closed
		///
		virtual void closeOrEnqueueForClosing(std::shared_ptr<core::objects::SessionInternals> session, int64_t closeGracePeriodInMillis) = 0;

		///
		/// Removes the given session for 'auto-closing' from this watchdog.
		///
		/// @param session the session to be removed.
		///
		virtual void dequeueFromClosing(std::shared_ptr<core::objects::SessionInternals> session) = 0;

		///
		/// Adds the given session proxy so that it will be automatically split the underlying session when the idle
		/// timeout or the maximum session duration is reached.
		///
		/// @param sessionProxy the session proxy to be added.
		///
		virtual void addToSplitByTimeout(std::shared_ptr<core::objects::ISessionProxy> sessionProxy) = 0;

		///
		/// Removes the given session proxy from automatically splitting it after idle or session max duration expired.
		///
		/// @param sessionProxy the session proxy to be removed.
		///
		virtual void removeFromSplitByTimeout(std::shared_ptr<core::objects::ISessionProxy> sessionProxy) = 0;
	};
}

#endif
