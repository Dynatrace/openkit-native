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

#ifndef CORE_BEACONSENDER_H
#define CORE_BEACONSENDER_H

#include <memory>
#include <future>

#include "communication/BeaconSendingContext.h"
#include "configuration/Configuration.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"

#include "core/objects/Session.h"

namespace core
{
	///
	/// The BeaconSender runs a thread executing the beacon sending states
	///
	class BeaconSender
	{
	public:
		///
		/// Default constructor
		/// @param[in] logger to write traces to
		/// @param[in] configuration general configuration options
		/// @param[in] httpClientProvider the provider for HTTPClient instances
		/// @param[in] timingProvider utility requried for timing related stuff
		///
		BeaconSender
		(
			std::shared_ptr<openkit::ILogger> logger,
			std::shared_ptr<configuration::Configuration> configuration,
			std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
			std::shared_ptr<providers::ITimingProvider> timingProvider
		);

		virtual ~BeaconSender() {}

		///
		/// Initialize this BeaconSender
		/// @return @c true if initialization succeeded, @c false otherwise
		///
		bool initialize();

		///
		/// Wait until initialization was completed.
		/// NOTE: this is a blocking operation
		/// @return @c true if initialization succeeded, @c false if initialization failed
		///
		bool waitForInit() const;

		///
		/// Wait until initialization was completed.  This function may return with @c false after the
		/// timeout occured when initialization is still in progress. The initialization may then finish after this call returned.
		/// This can be verified by checking @ref isInitialized() after some additional wait statement.
		/// NOTE: this is a blocking operation
		/// @param[in] timeoutMillis The maximum number of milliseconds to wait for initialization being completed.
		/// @return @c true if initialization succeeded, @c false if initialization failed or timeout occured
		///
		bool waitForInit(int64_t timeoutMillis) const;

		///
		/// Get a bool indicating whether OpenKit has been initialized or not.
		/// @returns @c true if @ref OpenKit has been initialized, @c false otherwise.
		bool isInitialized() const;

		///
		/// Shutdown this instance of the BeaconSender
		///
		void shutdown();

		///
		/// When starting a new Session, put it into open Sessions.
		/// A session is only put into the open Sessions if capturing is enabled.
		/// In case capturing is disabled, this method has no effect.
		/// @param[in] session Session to start
		///
		virtual void startSession(std::shared_ptr<core::objects::Session> session);

		///
		/// When finishing a Session, remove it from open Sessions and put it into finished Sessions.
		/// As soon as a session gets finished it will be transferred to the server.
		/// @param[in] session Session to finish
		///
		virtual void finishSession(std::shared_ptr<core::objects::Session> session);

	private:
		/// Logger to write traces to
		std::shared_ptr<openkit::ILogger> mLogger;

		/// Beacon sending context managing the state transitions and shutdown
		std::shared_ptr<communication::BeaconSendingContext> mBeaconSendingContext;

		/// thread instance running the beacon sending state machine
		std::future<bool> mSendingThread;

		/// timing provider for shutdown timeout
		std::shared_ptr<providers::ITimingProvider> mTimingProvider;
	};
}
#endif
