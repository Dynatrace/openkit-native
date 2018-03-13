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

#include "api/ISession.h"

#include "Action.h"
#include "util/SynchronizedQueue.h"

#include <memory>
#include <atomic>

namespace protocol
{
	class Beacon;
}

namespace core
{
	class BeaconSender;

	///
	///  Actual implementation of the ISession interface.
	///
	class Session : public api::ISession, private std::enable_shared_from_this<core::Session>
	{
	public:

		///
		/// Constructor
		/// @param[in] beaconSender beacon sender
		/// @param]in] beacon beacon used for serialization
		///
		Session(std::shared_ptr<BeaconSender> beaconSender, std::shared_ptr<protocol::Beacon> beacon);
			
		///
		/// Destructor
		///
		virtual ~Session() {}
		///
		/// Enters an Action with a specified name in this Session.
		/// @param[in] actionName name of the Action
		/// @returns Action instance to work with
		///
		virtual std::shared_ptr<api::IRootAction> enterAction(const UTF8String& actionName);

		///
		///  Ends this Session and marks it as ready for immediate sending.
		///
		virtual void end();

		///
		/// Returns the end time of the session
		/// @returns the end time of the session
		///
		int64_t getEndTime() const;

	private:

		///
		/// Return a flag if this session was ended already
		/// @returns @c true if session was already ended, @c false if session is still open
		///
		bool isSessionEnded() const;

	private:

		/// beacon sender
		std::shared_ptr<BeaconSender> mBeaconSender;

		/// beacon used for serialization
		std::shared_ptr<protocol::Beacon> mBeacon;

		/// end time
		std::atomic<int64_t> mEndTime;

		/// synchronized queue of root actions of this session
		util::SynchronizedQueue<std::shared_ptr<Action>> mOpenRootActions;
	};
}
#endif