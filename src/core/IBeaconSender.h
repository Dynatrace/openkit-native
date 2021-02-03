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

#ifndef _CORE_IBEACONSENDER_H
#define _CORE_IBEACONSENDER_H

#include "core/objects/SessionInternals.h"

namespace core
{
	class IBeaconSender
	{
	public:

		virtual ~IBeaconSender() = default;

		///
		/// Initialize this BeaconSender
		/// @return @c true if initialization succeeded, @c false otherwise
		///
		virtual bool initialize() = 0;

		///
		/// Wait until initialization was completed.
		/// NOTE: this is a blocking operation
		/// @return @c true if initialization succeeded, @c false if initialization failed
		///
		virtual bool waitForInit() const = 0;

		///
		/// Wait until initialization was completed.  This function may return with @c false after the
		/// timeout occured when initialization is still in progress. The initialization may then finish after this call returned.
		/// This can be verified by checking @ref isInitialized() after some additional wait statement.
		/// NOTE: this is a blocking operation
		/// @param[in] timeoutMillis The maximum number of milliseconds to wait for initialization being completed.
		/// @return @c true if initialization succeeded, @c false if initialization failed or timeout occured
		///
		virtual bool waitForInit(int64_t timeoutMillis) const = 0;

		///
		/// Get a bool indicating whether OpenKit has been initialized or not.
		/// @returns @c true if @ref OpenKit has been initialized, @c false otherwise.
		virtual bool isInitialized() const = 0;

		///
		/// Shutdown this instance of the BeaconSender
		///
		virtual void shutdown() = 0;

		///
		/// Returns the last stored IServerConfiguration
		///
		virtual std::shared_ptr<core::configuration::IServerConfiguration> getLastServerConfiguration() = 0;

		///
		/// Returns the current serverID to be used for creating new sessions.
		///
		virtual int32_t getCurrentServerID() const = 0;

		///
		/// Adds the given session to the known sessions of this beacon sender.
		///
		virtual void addSession(std::shared_ptr<core::objects::SessionInternals> session) = 0;
	};
}

#endif
