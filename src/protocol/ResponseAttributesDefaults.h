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

#ifndef _PROTOCOL_RESPONSEATTRIBUTESDEFAULTS_H
#define _PROTOCOL_RESPONSEATTRIBUTESDEFAULTS_H

#include "IResponseAttributes.h"
#include "ResponseAttribute.h"
#include "core/UTF8String.h"

#include <memory>

namespace protocol
{
	class ResponseAttributesDefaults
	{
	public:

		static const std::shared_ptr<IResponseAttributes> jsonResponse();
		static const std::shared_ptr<IResponseAttributes> keyValueResponse();
		static const std::shared_ptr<IResponseAttributes> undefined();

	private:
		static const core::UTF8String emptyString;
		
		class AbstractResponseDefaults : public IResponseAttributes
		{		
		public:

			int32_t getMaxBeaconSizeInBytes() const override = 0;

			int32_t getMaxSessionDurationInMilliseconds() const override = 0;

			int32_t getMaxEventsPerSession() const override = 0;

			int32_t getSendIntervalInMilliseconds() const override;

			int32_t getVisitStoreVersion() const override;

			bool isCapture() const override;

			bool isCaptureCrashes() const override;

			bool isCaptureErrors() const override;

			const core::UTF8String& getApplicationId() const override;
			
			int32_t getMultiplicity() const override;

			int32_t getServerId() const override;

			const core::UTF8String& getStatus() const override;

			int64_t getTimestampInMilliseconds() const override;

			bool isAttributeSet(ResponseAttribute attribute) const override;

			std::shared_ptr<IResponseAttributes> merge(std::shared_ptr<IResponseAttributes> attributes) const override;
		};

		class JsonResponseDefaults : public AbstractResponseDefaults
		{
			int32_t getMaxBeaconSizeInBytes() const override;

			int32_t getMaxSessionDurationInMilliseconds() const override;

			int32_t getMaxEventsPerSession() const override;

			int32_t getSessionTimeoutInMilliseconds() const override;
		};

		class KeyValueResponseDefaults : public AbstractResponseDefaults
		{
			int32_t getMaxBeaconSizeInBytes() const override;

			int32_t getMaxSessionDurationInMilliseconds() const override;

			int32_t getMaxEventsPerSession() const override;

			int32_t getSessionTimeoutInMilliseconds() const override;
		};

		class UndefinedDefaults : public AbstractResponseDefaults
		{
			int32_t getMaxBeaconSizeInBytes() const override;

			int32_t getMaxSessionDurationInMilliseconds() const override;

			int32_t getMaxEventsPerSession() const override;

			int32_t getSessionTimeoutInMilliseconds() const override;

			int32_t getServerId() const override;
		};
	};
}

#endif
