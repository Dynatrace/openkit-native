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

#ifndef CORE_BEACONSENDER_H
#define CORE_BEACONSENDER_H

#include "OpenKit/ILogger.h"
#include "communication/IBeaconSendingContext.h"
#include "core/IBeaconSender.h"
#include "core/configuration/IHTTPClientConfiguration.h"
#include "core/objects/SessionInternals.h"
#include "core/util/IInterruptibleThreadSuspender.h"
#include "core/util/ThreadSurrogate.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"

#include <memory>

namespace core
{
	///
	/// The BeaconSender runs a thread executing the beacon sending states
	///
	class BeaconSender
		: public IBeaconSender
	{
	public:
		///
		/// Default constructor
		/// @param[in] logger to write traces to
		/// @param[in] httpClientConfiguration initial HTTP client configuration.
		/// @param[in] httpClientProvider the provider for HTTPClient instances
		/// @param[in] timingProvider utility required for timing related stuff
		///
		BeaconSender
		(
			std::shared_ptr<openkit::ILogger> logger,
			std::shared_ptr<core::configuration::IHTTPClientConfiguration> httpClientConfiguration,
			std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
			std::shared_ptr<providers::ITimingProvider> timingProvider,
			std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender
		);

		~BeaconSender() override = default;

		bool initialize() override;

		bool waitForInit() const override;

		bool waitForInit(int64_t timeoutMillis) const override;

		bool isInitialized() const override;

		void shutdown() override;

		int32_t getCurrentServerID() const override;

		void addSession(std::shared_ptr<core::objects::SessionInternals> session) override;

	private:
		/// Logger to write traces to
		std::shared_ptr<openkit::ILogger> mLogger;

		/// Beacon sending context managing the state transitions and shutdown
		std::shared_ptr<communication::IBeaconSendingContext> mBeaconSendingContext;

		/// thread instance running the beacon sending state machine
		std::unique_ptr<core::util::ThreadSurrogate> mSendingThread;

		/// timing provider for shutdown timeout
		std::shared_ptr<providers::ITimingProvider> mTimingProvider;
	};
}
#endif
