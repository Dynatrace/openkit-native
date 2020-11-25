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


#ifndef _CORE_SESSIONWATCHDOGCONTEXT_H
#define _CORE_SESSIONWATCHDOGCONTEXT_H

#include "ISessionWatchdogContext.h"
#include "core/util/IInterruptibleThreadSuspender.h"
#include "core/util/SynchronizedQueue.h"
#include "providers/ITimingProvider.h"

#include <memory>
#include <atomic>
#include <chrono>
#include <vector>

namespace core
{
	class SessionWatchdogContext
		: public ISessionWatchdogContext
	{
	public:

		SessionWatchdogContext(std::shared_ptr<providers::ITimingProvider> timingProvider,
				std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender);

		~SessionWatchdogContext() override = default;

		///
		/// Returns the default sleep time if no session is to be split / closed.
		///
		static const std::chrono::milliseconds& getDefaultSleepTime();

		void execute() override;

		void requestShutdown() override;

		bool isShutdownRequested() override;

		void closeOrEnqueueForClosing(std::shared_ptr<core::objects::SessionInternals> session, int64_t closeGracePeriodInMillis) override;

		void dequeueFromClosing(std::shared_ptr<core::objects::SessionInternals> session) override;

		void addToSplitByTimeout(std::shared_ptr<core::objects::ISessionProxy> sessionProxy) override;

		void removeFromSplitByTimeout(std::shared_ptr<core::objects::ISessionProxy> sessionProxy) override;

		///
		/// Get a shallow copy of all sessions to close held by this context.
		/// 
		/// @remarks
		/// This method is inteded for unit tests.
		///
		std::vector<std::shared_ptr<core::objects::SessionInternals>> getSessionsToClose();

		///
		/// Get a shallow copy of all sessions to split by timeout held by this context.
		/// 
		/// @remarks
		/// This method is inteded for unit tests.
		///
		std::vector<std::shared_ptr<core::objects::ISessionProxy>> getSessionsToSplitByTimeout();

	private:

		int64_t closeExpiredSessions();

		int64_t splitTimedOutSessions();

		///
		/// Indicator whether shutdown was requested or not.
		///
		std::atomic<bool> mIsShutdown;

		///
		/// provider of the current time and for suspending the thread for a certain amount of time.
		///
		std::shared_ptr<providers::ITimingProvider> mTimingProvider;

		///
		/// instance for suspending the current thread for a given amount of time
		///
		std::shared_ptr<core::util::IInterruptibleThreadSuspender> mThreadSuspender;

		///
		/// holds all sessions which are to be closed after a certain grace period.
		///
		core::util::SynchronizedQueue<std::shared_ptr<core::objects::SessionInternals>> mSessionsToClose;

		///
		/// holds all session proxies which are to be split after expiration of either session duration or idle timeout.
		///
		core::util::SynchronizedQueue<std::shared_ptr<core::objects::ISessionProxy>> mSessionsToSplitByTimeout;
	};
}

#endif
