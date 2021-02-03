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
    , mSessionsToSplitByTimeout()
{
}

const std::chrono::milliseconds& SessionWatchdogContext::getDefaultSleepTime()
{
    return DEFAULT_SLEEP_TIME_MILLISECONDS;
}

void SessionWatchdogContext::execute()
{
	auto durationToNextCloseInMillis = closeExpiredSessions();
    auto durationToNextSplitInMillis = splitTimedOutSessions();

	mThreadSuspender->sleep(std::min(durationToNextCloseInMillis, durationToNextSplitInMillis));
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

void SessionWatchdogContext::addToSplitByTimeout(std::shared_ptr<core::objects::ISessionProxy> sessionProxy)
{
    if (sessionProxy->isFinished())
    {
        return;
    }

    mSessionsToSplitByTimeout.put(sessionProxy);
}

void SessionWatchdogContext::removeFromSplitByTimeout(std::shared_ptr<core::objects::ISessionProxy> sessionProxy)
{
    mSessionsToSplitByTimeout.remove(sessionProxy);
}

std::vector<std::shared_ptr<core::objects::SessionInternals>> SessionWatchdogContext::getSessionsToClose()
{
	return mSessionsToClose.toStdVector();
}

std::vector<std::shared_ptr<core::objects::ISessionProxy>> SessionWatchdogContext::getSessionsToSplitByTimeout()
{
    return mSessionsToSplitByTimeout.toStdVector();
}

int64_t SessionWatchdogContext::closeExpiredSessions()
{
    auto sleepTimeInMillis = DEFAULT_SLEEP_TIME_MILLISECONDS.count();
    auto allSessions = mSessionsToClose.toStdVector();
    std::list<std::shared_ptr<core::objects::SessionInternals>> closableSessions;

    // first iteration - get all closeable sessions
    for (auto& session : allSessions)
    {
        auto nowInMillis = mTimingProvider->provideTimestampInMilliseconds();
        auto gracePeriodEndTimeInMillis = session->getSplitByEventsGracePeriodEndTimeInMillis();
        auto isGracePeriodExpired = gracePeriodEndTimeInMillis <= nowInMillis;
        if (isGracePeriodExpired)
        {
            closableSessions.push_back(session);
            continue;
        }

        auto sleepTimeToGracePeriodEndInMillis = gracePeriodEndTimeInMillis - nowInMillis;
        sleepTimeInMillis = std::min(sleepTimeInMillis, sleepTimeToGracePeriodEndInMillis);
    }

    // remove the sessions
    for (auto& session : closableSessions)
    {
        mSessionsToClose.remove(session);
        session->end(false);
    }

    return sleepTimeInMillis;
}

int64_t SessionWatchdogContext::splitTimedOutSessions()
{
    auto sleepTimeInMillis = DEFAULT_SLEEP_TIME_MILLISECONDS.count();
    auto sessionProxiesToRemove = std::list<std::shared_ptr<core::objects::ISessionProxy>>();
    auto allSessionProxies = mSessionsToSplitByTimeout.toStdVector();

    // first iteration - get all session proxies that can be removed
    for(auto& sessionProxy : allSessionProxies)
    {
        auto nextSessionSplitInMillis = sessionProxy->splitSessionByTime();
        if (nextSessionSplitInMillis < 0)
        {
            sessionProxiesToRemove.push_back(sessionProxy);
            continue;
        }

        auto nowInMillis = mTimingProvider->provideTimestampInMilliseconds();
        auto durationToNextSplit = nextSessionSplitInMillis - nowInMillis;
        if (durationToNextSplit < 0)
        {
            continue;
        }

        sleepTimeInMillis = std::min(sleepTimeInMillis, durationToNextSplit);
    }

    // remove previously identified session proxies
    for(auto& sessionProxy : sessionProxiesToRemove)
    {
        mSessionsToSplitByTimeout.remove(sessionProxy);
    }

    return sleepTimeInMillis;
}