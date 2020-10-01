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

#include "SessionWatchdog.h"

using namespace core;

static constexpr int64_t SHUTDOWN_TIMEOUT_MILLIS = 2 * 1000; // 2 seconds shutdown timeout

SessionWatchdog::SessionWatchdog(std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::ISessionWatchdogContext> context)
	: mLogger(logger)
	, mSessionWatchdogThread(new core::util::ThreadSurrogate())
	, mContext(context)
{
}

void SessionWatchdog::initialize()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("SessionWatchdog initialize() - thread started");
	}

	mSessionWatchdogThread->start(std::bind(&SessionWatchdog::watchdogThreadFunction, this));
}

void SessionWatchdog::watchdogThreadFunction()
{
	while (!mContext->isShutdownRequested())
	{
		mContext->execute();
	}
}

void SessionWatchdog::shutdown()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("SessionWatchdog shutdown() - thread request shutdown");
	}

	mContext->requestShutdown();
	if (mSessionWatchdogThread->isAlive())
	{
		mSessionWatchdogThread->join(SHUTDOWN_TIMEOUT_MILLIS);
	}

	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("SessionWatchdog shutdown() - thread stopped");
	}
}

void SessionWatchdog::closeOrEnqueueForClosing(std::shared_ptr<core::objects::SessionInternals> session,
	int64_t closeGracePeriodInMillis)
{
	mContext->closeOrEnqueueForClosing(session, closeGracePeriodInMillis);
}

void SessionWatchdog::dequeueFromClosing(std::shared_ptr<core::objects::SessionInternals> session)
{
	mContext->dequeueFromClosing(session);
}
