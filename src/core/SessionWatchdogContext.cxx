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

#include "SessionWatchdogContext.h"

#include <algorithm>
#include <list>

using namespace core;

static const std::chrono::milliseconds DEFAULT_SLEEP_TIME_MILLISECONDS = std::chrono::seconds(5);

SessionWatchdogContext::SessionWatchdogContext(std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender)
	: mIsShutdown(false)
	, mTimingProvider(timingProvider)
	, mThreadSuspender(threadSuspender)
	, mSessionsToClose()
{
}

const std::chrono::milliseconds& SessionWatchdogContext::getDefaultSleepTime()
{
    return DEFAULT_SLEEP_TIME_MILLISECONDS;
}

void SessionWatchdogContext::execute()
{
	auto sleepTime = closeExpiredSessions();

	mThreadSuspender->sleep(sleepTime);
}

void SessionWatchdogContext::requestShutdown()
{
	mIsShutdown = true;
	mThreadSuspender->wakeup();
}

bool SessionWatchdogContext::isShutdownRequested()
{
	return mIsShutdown;
}

void SessionWatchdogContext::closeOrEnqueueForClosing(std::shared_ptr<core::objects::SessionInternals> session, int64_t closeGracePeriodInMillis)
{
    if (session->tryEnd())
    {
        return;
    }

    auto closeTime = mTimingProvider->provideTimestampInMilliseconds() + closeGracePeriodInMillis;
    session->setSplitByEventsGracePeriodEndTimeInMillis(closeTime);
    mSessionsToClose.put(session);
}

void SessionWatchdogContext::dequeueFromClosing(std::shared_ptr<core::objects::SessionInternals> session)
{
    mSessionsToClose.remove(session);
}

std::vector<std::shared_ptr<core::objects::SessionInternals>> SessionWatchdogContext::getSessionsToClose()
{
	return mSessionsToClose.toStdVector();
}

int64_t SessionWatchdogContext::closeExpiredSessions()
{
    auto sleepTime = DEFAULT_SLEEP_TIME_MILLISECONDS.count();
    auto allSessions = mSessionsToClose.toStdVector();
    std::list<std::shared_ptr<core::objects::SessionInternals>> closableSessions;

    // first iteration - get all closeable sessions
    for (auto session : allSessions)
    {
        auto now = mTimingProvider->provideTimestampInMilliseconds();
        auto gracePeriodEndTime = session->getSplitByEventsGracePeriodEndTimeInMillis();
        auto isGracePeriodExpired = gracePeriodEndTime <= now;
        if (isGracePeriodExpired)
        {
            closableSessions.push_back(session);
            continue;
        }

        auto sleepTimeToGracePeriodEnd = gracePeriodEndTime - now;
        sleepTime = std::min(sleepTime, sleepTimeToGracePeriodEnd);
    }

    for (auto session : closableSessions)
    {
        mSessionsToClose.remove(session);
        session->end();
    }

    return sleepTime;
}
