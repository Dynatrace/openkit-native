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

#include "BeaconSendingRequestUtil.h"
#include "BeaconSendingResponseUtil.h"

using namespace core::communication;
using namespace protocol;

std::shared_ptr<IStatusResponse> BeaconSendingRequestUtil::sendStatusRequest(IBeaconSendingContext& context, uint32_t numRetries, uint64_t initialRetryDelayInMillis)
{
	std::shared_ptr<IStatusResponse> statusResponse = nullptr;
	uint64_t sleepTimeInMillis = initialRetryDelayInMillis;
	uint32_t retry = 0;

	while (!context.isShutdownRequested())
	{
		std::shared_ptr<IHTTPClient> httpClient = context.getHTTPClient();
		if (httpClient == nullptr)
		{
			break;
		}

		statusResponse = httpClient->sendStatusRequest(context);
		if (BeaconSendingResponseUtil::isSuccessfulResponse(statusResponse)
			|| BeaconSendingResponseUtil::isTooManyRequestsResponse(statusResponse) // is handled by the states
			|| retry >= numRetries)
		{
			break;
		}

		// if no (valid) status response was received -> sleep and double the delay for each retry
		context.sleep(sleepTimeInMillis);

		sleepTimeInMillis *= 2;
		retry++;
	}

	return statusResponse;
}