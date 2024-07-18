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

#ifndef _CORE_OBJECTS_WEBREQUESTTRACER_H
#define _CORE_OBJECTS_WEBREQUESTTRACER_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor" // enable_shared_from_this has a public non virtual destructor throwing a false positive in this code
#endif

#include "OpenKit/IOpenKit.h"
#include "core/objects/IWebRequestTracerInternals.h"
#include "core/objects/OpenKitComposite.h"
#include "core/UTF8String.h"

#include <mutex>


namespace protocol
{
	class IBeacon;
}

namespace core
{
	namespace objects
	{
		///
		/// Implements a tracer for web requests
		///
		class WebRequestTracer
			: public IWebRequestTracerInternals
			, public std::enable_shared_from_this<WebRequestTracer>
		{
		public:

			///
			/// Constructor which can be used for tracing and timing of a web request handled by any 3rd party HTTP Client.
			/// Setting the Dynatrace tag to the @ref openkit::OpenKitConstants::WEBREQUEST_TAG_HEADER
			/// HTTP header has to be done manually by the user.
			///
			/// @param[in] logger to write traces to
			/// @param[in] parent the parent object, to which this web request belongs
			/// @param[in] beacon @ref protocol::Beacon used to serialize the @ref WebRequestTracer
			///
			WebRequestTracer(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<IOpenKitComposite> parent,
				std::shared_ptr<protocol::IBeacon> beacon,
				const core::UTF8String& url
			);

			static constexpr const char* UNKNOWN_URL = "<unknown>";

			///
			/// Test if given @c url contains a valid URL scheme according to RFC3986.
			/// @param[in] url The url to validate
			/// @return @c true on success, @c false otherwise.
			///
			static bool isValidURLScheme(const core::UTF8String& url);

			///
			/// Checks if the given @c url is valid and returns the URL excluding possible parameters.
			/// In case the url is invalid @ref UNKNOWN_URL is returned.
			///
			static core::UTF8String calculateUrlFrom(const core::UTF8String& url);

			const char* getTag() const override;

			std::shared_ptr<IWebRequestTracer> setBytesSent(int32_t bytesSent) override;

			std::shared_ptr<IWebRequestTracer> setBytesReceived(int32_t bytesReceived) override;

			std::shared_ptr<IWebRequestTracer> setBytesSent(int64_t bytesSent) override;

			std::shared_ptr<IWebRequestTracer> setBytesReceived(int64_t bytesReceived) override;

			std::shared_ptr<IWebRequestTracer> start() override;

			void stop(int32_t responseCode) override;

			void close() override;

			void cancel() override;

			///
			/// Returns the target URL of the web request
			/// @returns target URL of the web request
			///
			const core::UTF8String& getURL() const override;

			///
			/// Returns the response code of the web request
			/// @returns response code of the web request
			///
			int32_t getResponseCode() const override;

			///
			/// Returns the start time of the web request
			/// @returns start time of the web request
			///
			int64_t getStartTime() const override;

			///
			/// Returns the end time of the web request
			/// @returns end time of the web request
			///
			int64_t getEndTime() const override;

			///
			/// Returns the start sequence number
			/// @returns start sequence number of the web request
			///
			int32_t getStartSequenceNo() const override;

			///
			/// Returns the end sequence number
			/// @returns end sequence number of the web request
			///
			int32_t getEndSequenceNo() const override;

			///
			/// Returns the number of bytes sent
			/// @returns amount of bytes sent
			///
			int64_t getBytesSent() const override;

			///
			/// Returns the number of bytes received
			/// @returns amount of bytes received
			///
			int64_t getBytesReceived() const override;

			///
			/// Returns a flag if stop() has already been called
			/// @returns @c true if the web request tracer has been stopped already, @c false otherwise
			///
			bool isStopped() const override;

		private:

			///
			/// Stops the web request tracer.
			///
			/// @param responseCode HTTP response code to set
			/// @param discardData If @c true discard data and don't send it, if @c false send data.
			///
			void doStop(int32_t responseCode, bool discardData);

			///
			/// Returns a string describing the object, based on some important fields.
			/// This function is intended for debug printouts.
			/// @return a string describing the object
			///
			const std::string toString() const;

			/// Logger to write traces to
			const std::shared_ptr<openkit::ILogger> mLogger;

			///
			/// Parent object of this web request tracer.
			///
			const std::shared_ptr<IOpenKitComposite> mParent;

			///
			/// Mutex for synchronization
			///
			std::mutex mMutex;

			/// @ref protocol::Beacon used to serialize the WebRequestTracer
			const std::shared_ptr<protocol::IBeacon> mBeacon;

			/// ID of the @ref core::Action or @ref core::RootAction which is the parent of the WebRequestTracer
			const int32_t mParentActionID;

			/// The target URL of the web request
			const core::UTF8String mURL;

			/// Response code of the the web request
			int32_t mResponseCode;

			/// Number of bytes sent for the web request
			int64_t mBytesSent;

			/// Number of bytes received for the web request
			int64_t mBytesReceived;

			/// web request start time
			int64_t mStartTime;

			/// web request end time
			int64_t mEndTime;

			/// start sequence number
			int32_t mStartSequenceNo;

			/// end sequence number
			int32_t mEndSequenceNo;

			/// Dynatrace tag that has to be used for tracing the web request
			core::UTF8String mWebRequestTag;
		};
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
