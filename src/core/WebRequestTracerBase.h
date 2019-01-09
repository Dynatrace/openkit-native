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

#ifndef _CORE_WEBREQUESTTRACERBASE_H
#define _CORE_WEBREQUESTTRACERBASE_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/IOpenKit.h"
#include "OpenKit/IWebRequestTracer.h"

#include <atomic>

#include "core/UTF8String.h"

namespace protocol
{
	class Beacon;
}

namespace core
{
	///
	/// Base class for WebRequest tracers.
	///
	class WebRequestTracerBase : public openkit::IWebRequestTracer, public std::enable_shared_from_this<WebRequestTracerBase>
	{
	public:

		///
		/// Constructor taking an @ref core::Action
		/// @param[in] logger to write traces to
		/// @param[in] beacon @ref protocol::Beacon used to serialize the @ref WebRequestTracerBase
		/// @param[in] parentActionID id of the parent of the WebRequestTracer
		///
		WebRequestTracerBase(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<protocol::Beacon> beacon, int32_t parentActionID);

		const char* getTag() const override;

		virtual std::shared_ptr<IWebRequestTracer> setResponseCode(int32_t responseCode) override;

		virtual std::shared_ptr<IWebRequestTracer> setBytesSent(int32_t bytesSent) override;

		virtual std::shared_ptr<IWebRequestTracer> setBytesReceived(int32_t bytesReceived) override;

		virtual std::shared_ptr<IWebRequestTracer> start() override;

		virtual void stop() override;

		///
		/// Returns the target URL of the web request
		/// @returns target URL of the web request
		///
		const core::UTF8String getURL() const;

		///
		/// Returns the response code of the web request
		/// @returns response code of the web request 
		///
		virtual int32_t getResponseCode() const;

		///
		/// Returns the start time of the web request
		/// @returns start time of the web request 
		///
		int64_t getStartTime() const;

		///
		/// Returns the end time of the web request
		/// @returns end time of the web request 
		///
		int64_t getEndTime() const;

		///
		/// Returns the start sequence number
		/// @returns start sequence number of the web request 
		///
		int32_t getStartSequenceNo() const;

		///
		/// Returns the end sequence number
		/// @returns end sequence number of the web request 
		///
		int32_t getEndSequenceNo() const;

		///
		/// Returns the number of bytes sent 
		/// @returns amount of bytes sent
		///
		virtual int32_t getBytesSent() const;

		///
		/// Returns the number of bytes received 
		/// @returns amount of bytes received
		///
		virtual int32_t getBytesReceived() const;

		///
		/// Returns a flag if stop() has already been called
		/// @returns @c true if the web request tracer has been stopped already, @c false otherwise
		///
		bool isStopped() const;

	private:
		///
		/// Returns a string describing the object, based on some important fields.
		/// This function is indended for debug printouts.
		/// @return a string describing the object
		///
		const std::string toString() const;

		/// Logger to write traces to
		std::shared_ptr<openkit::ILogger> mLogger;

		/// @ref protocol::Beacon used to serialize the WebRequestTracer
		std::shared_ptr<protocol::Beacon> mBeacon;

		/// ID of the @ref core::Action or @ref core::RootAction which is the parent of the WebRequestTracer
		int32_t mParentActionID;

		/// Response code of the the web request
		int32_t mResponseCode;

		/// Number of bytes sent for the web request
		int32_t mBytesSent;

		/// Number of bytes received for the web request
		int32_t mBytesReceived;

		/// web request start time
		int64_t mStartTime;

		/// web request end time
		std::atomic<int64_t> mEndTime;

		/// start sequence number
		int32_t mStartSequenceNo;
		
		/// end sequence number
		int32_t mEndSequenceNo;

		/// Dynatrace tag that has to be used for tracing the web request
		UTF8String mWebRequestTag;

	protected:
		/// The target URL of the web request
		UTF8String mURL;
	};
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#endif

#endif
