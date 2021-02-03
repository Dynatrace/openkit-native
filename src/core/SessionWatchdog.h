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


#ifndef _CORE_SESSIONWATCHDOG_H
#define _CORE_SESSIONWATCHDOG_H

#include "OpenKit/ILogger.h"

#include "ISessionWatchdog.h"
#include "ISessionWatchdogContext.h"
#include "core/util/ThreadSurrogate.h"

#include <memory>

namespace core
{
	class SessionWatchdog
		: public ISessionWatchdog
	{
	public:

		SessionWatchdog(std::shared_ptr<openkit::ILogger> logger,
			std::shared_ptr<core::ISessionWatchdogContext> context);

		~SessionWatchdog() override = default;

		void initialize() override;

		void shutdown() override;

		void closeOrEnqueueForClosing(std::shared_ptr<core::objects::SessionInternals> session, int64_t closeGracePeriodInMillis) override;

		void dequeueFromClosing(std::shared_ptr<core::objects::SessionInternals> session) override;

		void addToSplitByTimeout(std::shared_ptr<core::objects::ISessionProxy> sessionProxy) override;

		void removeFromSplitByTimeout(std::shared_ptr<core::objects::ISessionProxy> sessionProxy) override;

	private:

		void watchdogThreadFunction();

		std::shared_ptr<openkit::ILogger> mLogger;

		std::unique_ptr<core::util::ThreadSurrogate> mSessionWatchdogThread;

		///
		/// context holding sessions to be closed after a grace period and sessions to be split after idle/max timeout
		///
		std::shared_ptr<core::ISessionWatchdogContext> mContext;

	};
}

#endif
