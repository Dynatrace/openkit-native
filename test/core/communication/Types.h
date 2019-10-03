/**
 * Copyright 2018-2019 Dynatrace LLC
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

#ifndef _TEST_CORE_COMMUNICATION_TYPES_H
#define _TEST_CORE_COMMUNICATION_TYPES_H

#include "core/communication/AbstractBeaconSendingState.h"
#include "core/communication/BeaconSendingCaptureOffState.h"
#include "core/communication/BeaconSendingCaptureOnState.h"
#include "core/communication/BeaconSendingContext.h"
#include "core/communication/BeaconSendingFlushSessionsState.h"
#include "core/communication/BeaconSendingInitialState.h"
#include "core/communication/BeaconSendingRequestUtil.h"
#include "core/communication/BeaconSendingResponseUtil.h"
#include "core/communication/BeaconSendingTerminalState.h"

namespace test
{
	namespace types
	{
		using AbstractBeaconSendingState_t = core::communication::AbstractBeaconSendingState;
		using AbstractBeaconSendingState_sp = std::shared_ptr<AbstractBeaconSendingState_t>;

		using BeaconSendingCaptureOffState_t = core::communication::BeaconSendingCaptureOffState;
		using BeaconSendingCaptureOffState_sp = std::shared_ptr<BeaconSendingCaptureOffState_t>;

		using BeaconSendingCaptureOnState_t = core::communication::BeaconSendingCaptureOnState;
		using BeaconSendingCaptureOnState_sp = std::shared_ptr<BeaconSendingCaptureOnState_t>;

		using BeaconSendingContext_t = core::communication::BeaconSendingContext;
		using BeaconSendingContext_sp = std::shared_ptr<BeaconSendingContext_t>;

		using BeaconSendingFlushSessionState_t = core::communication::BeaconSendingFlushSessionsState;
		using BeaconSendingFlushSessionState_sp = std::shared_ptr<BeaconSendingFlushSessionState_t>;

		using BeaconSendingInitialState_t = core::communication::BeaconSendingInitialState;
		using BeaconSendingInitialState_sp = std::shared_ptr<BeaconSendingInitialState_t>;

		using BeaconSendingRequestUtil_t = core::communication::BeaconSendingRequestUtil;
		using BeaconSendingResponseUtil_t = core::communication::BeaconSendingResponseUtil;

		using BeaconSendingTerminalState_t = core::communication::BeaconSendingTerminalState;
		using BeaconSendingTerminalState_sp = std::shared_ptr<BeaconSendingTerminalState_t>;
	}
}

#endif
