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

#ifndef _PROTOCOL_IBEACONINITIALIZER_H
#define _PROTOCOL_IBEACONINITIALIZER_H

#include "OpenKit/ILogger.h"

#include "core/UTF8String.h"
#include "core/caching/IBeaconCache.h"
#include "providers/ISessionIDProvider.h"
#include "providers/ITimingProvider.h"
#include "providers/IThreadIDProvider.h"
#include "providers/IPRNGenerator.h"

#include <memory>
#include <cstdint>

namespace protocol
{
	class IBeaconInitializer
	{
	public:

		///
		/// Destructor
		///
		virtual ~IBeaconInitializer() = default;

		///
		/// Returns the logger for reporting messages.
		///
		virtual std::shared_ptr<openkit::ILogger> getLogger() const = 0;

		///
		/// Returns the cache where the data of the beacon is stored until it gets sent.
		///
		virtual std::shared_ptr<core::caching::IBeaconCache> getBeaconCache() const = 0;

		///
		/// Returns @c true if IBeaconInitializer::getClientIpAddress shall be transmitted to the server.
		///
		virtual bool useClientIpAddress() const = 0;

		///
		/// Returns the client IP address of the session / beacon.
		///
		virtual const core::UTF8String& getClientIpAddress() const = 0;

		///
		/// Returns the ISessionIDProvider to obtain the identifier for a session / beacon.
		///
		virtual std::shared_ptr<providers::ISessionIDProvider> getSessionIdProvider() const = 0;

		///
		/// Returns the sequence number for the beacon/session for identification in case of session split by events.
        /// The session sequence number complements the session ID.
		///
		virtual int32_t getSessionSequenceNumber() const = 0;

		///
		/// Returns the IThreadIDProvider to obtain the identifier of the current thread.
		///
		virtual std::shared_ptr<providers::IThreadIDProvider> getThreadIdProvider() const = 0;

		///
		/// Returns the ITimingProvider to obtain the current timestamp.
		///
		virtual std::shared_ptr<providers::ITimingProvider> getTiminigProvider() const = 0;

		///
		/// Returns the IPRNGenerator to obtain random numbers (e.g. for randomizing device IDs).
		///
		virtual std::shared_ptr<providers::IPRNGenerator> getRandomNumberGenerator() const = 0;
	};
}

#endif
