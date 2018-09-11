/**
* Copyright 2018 Dynatrace LLC
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

#ifndef _CORE_SESSION_H
#define _CORE_SESSION_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/ISession.h"
#include "OpenKit/IRootAction.h"
#include "OpenKit/ILogger.h"
#include "NullRootAction.h"

#include "UTF8String.h"
#include "util/SynchronizedQueue.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/IHTTPClientProvider.h"
#include "configuration/BeaconConfiguration.h"

#include <memory>
#include <atomic>

namespace protocol
{
	class Beacon;
}

namespace core
{
	class BeaconSender;

	class RootAction;

	///
	///  Actual implementation of the ISession interface.
	///
	class Session : public openkit::ISession, public std::enable_shared_from_this<core::Session>
	{
	public:

		///
		/// Constructor
		/// @param[in] logger to write traces to
		/// @param[in] beaconSender beacon sender
		/// @param[in] beacon beacon used for serialization
		///
		Session(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<BeaconSender> beaconSender, std::shared_ptr<protocol::Beacon> beacon);
			
		///
		/// Destructor
		///
		virtual ~Session() {}

		virtual std::shared_ptr<openkit::IRootAction> enterAction(const char* actionName) override;

		virtual void identifyUser(const char* userTag) override;

		virtual void reportCrash(const char* errorName, const char* reason, const char* stacktrace) override;

		virtual std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url) override;

		virtual void end() override;

		///
		/// Returns the end time of the session
		/// @returns the end time of the session
		///
		virtual int64_t getEndTime() const;


		///
		/// Method to be called by the child action upon the call of leaveAction
		/// @param[in] rootAction RootAction that was closed
		///
		void rootActionEnded(std::shared_ptr<RootAction> rootAction);

		///
		/// Start a session
		///
		void startSession();

		///
		/// Sends the current Beacon state
		/// @param[in] clientProvider the IHTTPClientProvider to use for sending
		/// @returns the status response returned for the Beacon data
		///
		virtual std::shared_ptr<protocol::StatusResponse> sendBeacon(std::shared_ptr<providers::IHTTPClientProvider> clientProvider);

		///
		/// Test if this session is empty or not
		///
		/// A session is considered to be empty, if it does not contain any action or event data.
		/// @returns @c true if the session is empty, @c false otherwise
		///
		virtual bool isEmpty() const;


		///
		/// Clears data that has been captured so far.
		///
		/// This is called, when capturing is turned off to avoid having too much data.
		///
		virtual void clearCapturedData();

		///
		/// Return a flag if this session was ended already
		/// @returns @c true if session was already ended, @c false if session is still open
		///
		bool isSessionEnded() const;

		///
		/// Sets the beacon configuration
		/// @param[in] beaconConfiguration the beacon configuration to apply to the Beacon
		///
		virtual void setBeaconConfiguration(std::shared_ptr<configuration::BeaconConfiguration> beaconConfiguration);

		///
		/// Returns the beacon configuration
		/// @returns the beacon configuration
		///
		virtual std::shared_ptr<configuration::BeaconConfiguration> getBeaconConfiguration() const;

	private:
		///
		/// Returns a string describing the object, based on some important fields.
		/// This function is indended for debug printouts.
		/// @return a string describing the object
		///
		const std::string toString() const;

		/// Logger to write traces to
		std::shared_ptr<openkit::ILogger> mLogger;

		/// beacon sender
		std::shared_ptr<BeaconSender> mBeaconSender;

		/// beacon used for serialization
		std::shared_ptr<protocol::Beacon> mBeacon;

		/// end time
		std::atomic<int64_t> mEndTime;

		/// synchronized queue of root actions of this session
		util::SynchronizedQueue<std::shared_ptr<openkit::IRootAction>> mOpenRootActions;

		/// instance of NullRootAction
		std::shared_ptr<NullRootAction> NULL_ROOT_ACTION;

		/// Null WebRequestTracer
		static std::shared_ptr<NullWebRequestTracer> NULL_WEB_REQUEST_TRACER;
	};
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#endif

#endif
