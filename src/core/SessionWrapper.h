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

#ifndef _CORE_SESSIONWRAPPER_H
#define _CORE_SESSIONWRAPPER_H

#include "core/objects/Session.h"
#include "protocol/IStatusResponse.h"

#include <memory>

namespace core
{
	namespace configuration
	{
		class IBeaconConfiguration;
	}
}

namespace core
{
	///
	/// Wrapper around the @ref core::Session which holds additional data
	/// required only in the communication package
	class SessionWrapper
	{
	public:

		///
		/// Constructor taking a session object
		/// @param[in] session pointer
		///
		SessionWrapper(std::shared_ptr<core::objects::Session> session);

		///
		/// Updates the @ref core::configuration::BeaconConfiguration in the wrapped session.
		/// Besides updating the beacon configuration this also sets the @ref beaconConfigurationSet to  @c true.
		/// @param[in] beaconConfiguration the beacon configuration to update the session with
		///
		void updateBeaconConfiguration(std::shared_ptr<core::configuration::IBeaconConfiguration> beaconConfiguration);

		///
		/// Returns the beacon configuration
		/// @returns the current beacon configuration
		///
		std::shared_ptr<core::configuration::IBeaconConfiguration> getBeaconConfiguration() const;

		///
		/// Returns a flag if the beacon configuration of the session has been set at least once
		/// @returns flag if the configuration has been set
		///
		bool isBeaconConfigurationSet() const;

		///
		/// Finishes this session
		/// This only sets a finished flag to @c true since this will be called by
		/// the session itself
		///
		void finishSession();

		///
		/// Get a  boolean flag indicating whether this session has been finished or not.
		/// @return @c true if this session has been finished , @c false otherwise
		///
		bool isSessionFinished() const;

		///
		/// Will be called each time a new session request was made for a session.
		///
		void decreaseNumberOfNewSessionRequests();

		///
		/// Get a flag indicating whether new session request can be sent, or not.
		/// @returns @c true if the new session request can be sent, @c false otherwise.
		///
		bool canSendNewSessionRequest() const;

		///
		/// Test if the Session is empty.
		/// @returns flag if the wrapped session indicates that data is present
		///
		bool isEmpty() const;

		///
		/// Ends the session
		///
		void end();

		///
		/// Gets the wrapped session
		/// @returns the wrapped session
		///
		std::shared_ptr<core::objects::Session> getWrappedSession() const;

		///
		/// Get a boolean value indicating whether data sending is allowed or not.
		/// If the multiplicity is greater 0 sending is allowed
		/// @returns @c true if beacon data may be send, @c false otherwise
		///
		bool isDataSendingAllowed() const;

		///
		/// Clear the sessions data
		///
		void clearCapturedData();

		///
		/// Send beacon forward call
		/// @param[in] httpClientProvider http client provider
		/// @returns the status response
		///
		std::shared_ptr<protocol::IStatusResponse> sendBeacon(std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider);

	private:

		/// pointer to wrapped session
		std::shared_ptr<core::objects::Session> mWrappedSession;

		/// the beacon configuration used by the session
		std::shared_ptr<configuration::IBeaconConfiguration> mBeaconConfiguration;

		/// atomic flag if the beacon configuration has been set
		std::atomic<bool> mIsBeaconConfigurationSet;

		/// atomic flag if the session has been ended
		std::atomic<bool> mSessionFinished;

		/// number of remaining session requests before giving up
		uint32_t mNumNewSessionRequestsLeft;
	};
}

#endif
