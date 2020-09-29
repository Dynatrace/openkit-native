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

#ifndef _TEST_COMMUNICATION_CUSTOMMATCHERS_H
#define _TEST_COMMUNICATION_CUSTOMMATCHERS_H

#include "core/communication/IBeaconSendingState.h"

#include "gmock/gmock.h"

/// Converts enum to integer for debug printout
template <typename Enumeration>
auto as_integer(Enumeration const value)
-> typename std::underlying_type<Enumeration>::type
{
	return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

/// Checks if the argument (of type @ref AbstractBeaconSendingState) is of StateType BEACON_SENDING_INIT_STATE
MATCHER(IsABeaconSendingInitState, "")
{
	if (arg->getStateType() == core::communication::IBeaconSendingState::StateType::BEACON_SENDING_INIT_STATE)
	{
		return true;
	}
	*result_listener << "Unexpected stateType " << as_integer(arg->getStateType());

	return false;
}

/// Checks if the argument (of type @ref AbstractBeaconSendingState) is of StateType BEACON_SENDING_CAPTURE_ON_STATE
MATCHER(IsABeaconSendingCaptureOnState, "")
{
	if (arg->getStateType() == core::communication::IBeaconSendingState::StateType::BEACON_SENDING_CAPTURE_ON_STATE)
	{
		return true;
	}
	*result_listener << "Unexpected stateType " << as_integer(arg->getStateType());

	return false;
}

/// Checks if the argument (of type @ref AbstractBeaconSendingState) is of StateType BEACON_SENDING_CAPTURE_OFF_STATE
MATCHER(IsABeaconSendingCaptureOffState, "")
{
	if (arg->getStateType() == core::communication::IBeaconSendingState::StateType::BEACON_SENDING_CAPTURE_OFF_STATE)
	{
		return true;
	}
	*result_listener << "Unexpected stateType " << as_integer(arg->getStateType());

	return false;
}

/// Checks if the argument (of type @ref AbstractBeaconSendingState) is of StateType BEACON_SENDING_FLUSH_SESSIONS_STATE
MATCHER(IsABeaconSendingFlushSessionsState, "")
{
	if (arg->getStateType() == core::communication::IBeaconSendingState::StateType::BEACON_SENDING_FLUSH_SESSIONS_STATE)
	{
		return true;
	}
	*result_listener << "Unexpected stateType " << as_integer(arg->getStateType());

	return false;
}

/// Checks if the argument (of type @ref AbstractBeaconSendingState) is of StateType BEACON_SENDING_TERMINAL_STATE
MATCHER(IsABeaconSendingTerminalState, "")
{
	if (arg->getStateType() == core::communication::IBeaconSendingState::StateType::BEACON_SENDING_TERMINAL_STATE)
	{
		return true;
	}
	*result_listener << "Unexpected stateType " << as_integer(arg->getStateType());

	return false;
}

#endif