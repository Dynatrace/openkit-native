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

#ifndef _CORE_PROTOCOL_IRESPONSEATTRIBUTES_H
#define _CORE_PROTOCOL_IRESPONSEATTRIBUTES_H

#include "ResponseAttribute.h"
#include "core/UTF8String.h"

#include <memory>

namespace protocol
{
	///
	/// Defines the attributes received as response form the server.
	///
	class IResponseAttributes
	{
	public:

		virtual ~IResponseAttributes() = default;

		///
		/// Returns the maximum POST body size when sending beacons
		///
		virtual int32_t getMaxBeaconSizeInBytes() const = 0;

		///
		/// Returns the maximum duration in milliseconds after which a session will be split.
		///
		virtual int32_t getMaxSessionDurationInMilliseconds() const = 0;

		///
		/// Returns the maximum number of top level actions after which a session will be split.
		///
		virtual int32_t getMaxEventsPerSession() const = 0;

		///
		/// Returns the idle timeout in milliseconds after which a session will be split.
		///
		virtual int32_t getSessionTimeoutInMilliseconds() const = 0;

		///
		/// Returns the send interval in milliseconds.
		///
		virtual int32_t getSendIntervalInMilliseconds() const = 0;

		///
		/// Returns the version of the visit store to be used.
		///
		virtual int32_t getVisitStoreVersion() const = 0;

		///
		/// Indicator whether capturing data is generally allowed or not.
		///
		virtual bool isCapture() const = 0;

		///
		/// Indicator whether crashes should be captured or not.
		///
		virtual bool isCaptureCrashes() const = 0;

		///
		/// Indicator whether errors should be captured or not.
		///
		virtual bool isCaptureErrors() const = 0;

		///
		/// Gets a session sampling value in percent.
		///
		/// @par
		/// Value returned here is configured per application as Cost Control.
		///
		virtual int32_t  getTrafficControlPercentage() const = 0;

		///
		/// Returns the application UUID to which this configuration belongs to.
		///
		virtual const core::UTF8String& getApplicationId() const = 0;

		///
		/// Returns the multiplicity
		///
		virtual int32_t getMultiplicity() const = 0;

		///
		/// Returns the ID of the server to where all data should be sent.
		///
		virtual int32_t getServerId() const = 0;

		///
		///  Returns the status of the new session configuration request.
		///
		virtual const core::UTF8String& getStatus() const = 0;
		
		///
		/// Returns the timestamp of the attributes which were returned by the server.
		///
		virtual int64_t getTimestampInMilliseconds() const = 0;

		///
		/// Checks whether the given attribute was set/sent by the server.
		///
		/// @param attribute  the attribute to be checked if it was sent by the server.
		/// @return @c true if the given attribute was sent from the server, @c false otherwise.
		///
		virtual bool isAttributeSet(ResponseAttribute attribute) const = 0;

		///
		/// Creates a new response attributes object by merging the given attributes with this one. Single attributes are
		/// selectively taken over from the given response as long as the respective attribute has @ref isAttributeSet
		/// return @c true.
		///
		/// @param attributes the response attributes which will be merged together with this attributes into a new response
		/// attributes object.
		/// @return a new response attributes instance as merge of this and the given object.
		virtual std::shared_ptr<IResponseAttributes> merge(std::shared_ptr<IResponseAttributes> attributes) const = 0;
	};
}

#endif
