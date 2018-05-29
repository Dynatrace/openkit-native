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

#ifndef _CORE_ACTIONCOMMONIMPL_H
#define _CORE_ACTIONCOMMONIMPL_H

#include "OpenKit/ILogger.h"
#include "OpenKit/IWebRequestTracer.h"
#include "core/NullWebRequestTracer.h"
#include <memory>

namespace protocol
{
	class Beacon;
}

namespace core
{
	///
	/// Wrapper/PIMPL for functions used by Action and RootAction to avoid code duplication
	///
	class ActionCommonImpl
	{
	public:

		///
		/// Constructor
		/// @param[in] logger logger instance to use
		/// @param[in] beacon for this session that will serialize the data
		/// @param[in] actionID integer ID of the action this @ref ActionCommonImpl will create data for
		/// @param[in] objectID instance details serialization used for logging
		///
		ActionCommonImpl(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<protocol::Beacon> beacon, int32_t actionID, const std::string& objectID);

		///
		/// Add event (aka. named event) to Beacon.
		/// @param eventName Event's name.
		///
		void reportEvent(const char* eventName);

		///
		/// Add key-value-pair to Beacon.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		void reportValue(const char* valueName, int32_t value);

		///
		/// Add key-value-pair to Beacon.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		void reportValue(const char* valueName, double value);

		///
		/// Add key-value-pair to Beacon.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		void reportValue(const char* valueName, const char* value);
	
		///
		/// Add error to Beacon.
		/// @param errorName Error's name.
		/// @param errorCode Some error code.
		/// @param reason Reason for that error.
		///
		void reportError(const char* errorName, int32_t errorCode, const char* reason);

		///
		/// Add web request to Beacon
		/// @param[in] url the url used by the WebRequestTracer
		///
		std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* url);

	private:
		/// logger instance
		std::shared_ptr<openkit::ILogger> mLogger;

		/// beacon collection and sending this session's data
		std::shared_ptr<protocol::Beacon> mBeacon;

		/// the action ID
		int32_t mActionID;

		/// object information
		const std::string mObjectID;

	public:

		/// Null WebRequestTracer
		static std::shared_ptr<NullWebRequestTracer> NULL_WEB_REQUEST_TRACER;

	};
}

#endif