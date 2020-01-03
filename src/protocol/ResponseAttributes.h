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

#ifndef _PROTOCOL_RESPONSEATTRIBUTES_H
#define _PROTOCOL_RESPONSEATTRIBUTES_H

#include "IResponseAttributes.h"
#include "ResponseAttribute.h"
#include "core/util/EnumClassHash.h"

#include <unordered_set>
#include <memory>

namespace protocol
{
	///
	/// Implements @ref protocol::IResponseAttributes providing all attributes received from the server.
	///
	class ResponseAttributes : public IResponseAttributes
	{
	public:
		using AttributeSet = std::unordered_set<protocol::ResponseAttribute, core::util::EnumClassHash>;

		class Builder
		{
		public:

			Builder(const IResponseAttributes& defaults);

			AttributeSet& getSetAttributes();

			int32_t getMaxBeaconSizeInBytes() const;

			///
			/// Sets the maximum size in bytes.
			///
			/// @param maxBeaconSizeInBytes the maximum size in bytes when sending beacon data.
			/// @return @c this
			///
			Builder& withMaxBeaconSizeInBytes(int32_t maxBeaconSizeInBytes);

			int32_t getMaxSessionDurationInMilliseconds() const;

			///
			/// Sets the maximum duration after which a session is to be split.
			///
			/// @param maxSessionDurationInMilliseconds maximum duration of a session in milliseconds
			/// @return @c this
			///
			Builder& withMaxSessionDurationInMilliseconds(int32_t maxSessionDurationInMilliseconds);

			int32_t getMaxEventsPerSession() const;

			///
			/// Sets the maximum number of top level actions after which a session is to be split.
			///
			/// @param maxEventsPerSession the maximum number of top level actions
			/// @return @c this
			///
			Builder& withMaxEventsPerSession(int32_t maxEventsPerSession);

			int32_t getSessionTimeoutInMilliseconds() const;

			///
			/// Sets the idle timeout after which a session si to be split.
			///
			/// @param sessionTimeoutInMilliseconds the maximum idle timeout of a session in milliseconds
			/// @return @c this
			///
			Builder& withSessionTimeoutInMilliseconds(int32_t sessionTimeoutInMilliseconds);

			int32_t getSendIntervalInMilliseconds() const;

			///
			/// Sets the send interval in milliseconds at which beacon data is sent to the server.
			///
			/// @param sendIntervalInMilliseconds the send interval in milliseconds
			/// @return @ this
			///
			Builder& withSendIntervalInMilliseconds(int32_t sendIntervalInMilliseconds);

			int32_t getVisitStoreVersion() const;

			///
			/// Sets the version of the visit store that should be used.
			///
			/// @param visitStoreVersion the version of the visit store
			/// @return @ this
			///
			Builder& withVisitStoreVersion(int32_t visitStoreVersion);

			bool isCapture() const;

			///
			/// Sets whether capturing is enabled/disabled.
			///
			/// @param capture capture state
			/// @return @ this
			Builder& withCapture(bool capture);

			bool isCaptureCrashes() const;

			///
			/// Sets whether capturing crashes is enabled/disabled.
			///
			/// @param captureCrashes crash capture state
			/// @return @ this
			///
			Builder& withCaptureCrashes(bool captureCrashes);

			bool isCaptureErrors() const;

			///
			/// sets whether capturing of errors is enabled/disabled.
			///
			/// @param captureErrors error capture state.
			/// @return @c this
			///
			Builder& withCaptureErrors(bool captureErrors);

			int32_t getMultiplicity() const;

			///
			/// Sets the multiplicity
			///
			/// @param multiplicity the multiplicity to set
			/// @return @c this
			///
			Builder& withMultiplicity(int32_t multiplicity);

			int32_t getServerId() const;

			///
			/// Sets the ID of the server to which data should be sent to.
			///
			/// @param serverId the ID of the server to communicate with
			/// @return @c this
			///
			Builder& withServerId(int32_t serverId);

			int64_t getTimestampInMilliseconds() const;

			///
			/// Sets the timestamp of the configuration sent by the server.
			///
			/// @param timestampInMilliseconds the timestamp of the configuration in milliseconds
			/// @return @c this
			///
			Builder& withTimestampInMilliseconds(int64_t timestampInMilliseconds);

			///
			/// Creates a new @ref protocol::IResponseAttributes instance with all the attributes sent in this builder.
			///
			std::shared_ptr<protocol::IResponseAttributes> build();

		private:

			void setAttribute(protocol::ResponseAttribute attribute);

			AttributeSet mSetAttributes;

			int32_t mMaxBeaconSizeInBytes;
			int32_t mMaxSessionDurationInMilliseconds;
			int32_t mMaxEventsPerSession;
			int32_t mSessionTimeoutInMilliseconds;
			int32_t mSendIntervalInMilliseconds;
			int32_t mVisitStoreVersion;

			bool mIsCapture;
			bool mIsCaptureCrashes;
			bool mIsCaptureErrors;

			int32_t mMultiplicity;
			int32_t mServerId;

			int64_t mTimestampInMilliseconds;
		};

		///
		/// Creates a e new response attributes instance from the given builder
		///
		/// @param builder the builder from which to create the new instance
		///
		ResponseAttributes(Builder& builder);

		///
		/// Creates a new builder initialized with the default values for key-value parsing.
		///
		static std::shared_ptr<Builder> withKeyValueDefaults();

		///
		/// Creates a new builder instance with default values for JSON parsing.
		///
		static std::shared_ptr<Builder> withJsonDefaults();

		///
		/// Creates a new builder instance with undefined default values.
		///
		static std::shared_ptr<Builder> withUndefinedDefaults();

		int32_t getMaxBeaconSizeInBytes() const override;

		int32_t getMaxSessionDurationInMilliseconds() const override;

		int32_t getMaxEventsPerSession() const override;

		int32_t getSessionTimeoutInMilliseconds() const override;

		int32_t getSendIntervalInMilliseconds() const override;

		int32_t getVisitStoreVersion() const override;

		bool isCapture() const override;

		bool isCaptureCrashes() const override;

		bool isCaptureErrors() const override;

		int32_t getMultiplicity() const override;

		int32_t getServerId() const override;

		int64_t getTimestampInMilliseconds() const override;

		bool isAttributeSet(protocol::ResponseAttribute attribute) const override;

		std::shared_ptr<IResponseAttributes> merge(std::shared_ptr<IResponseAttributes> attributes) const override;

	private:

		static inline void applyBeaconSize(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline void applySessionDuration(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline void applyEventsPerSession(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline void applySessionTimeout(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline void applySendInterval(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline  void applyVisitStoreVersion(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline void applyCapture(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline void applyCaptureCrashes(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline void applyCaptureErrors(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline void applyMultiplicity(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline void applyServerId(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		static inline void applyTimestamp(Builder& builder, std::shared_ptr<IResponseAttributes> attributes);

		AttributeSet mSetAttributes;
		int32_t mMaxBeaconSizeInBytes;
		int32_t mMaxSessionDurationInMilliseconds;
		int32_t mMaxEventsPerSession;
		int32_t mSessionTimeoutInMilliseconds;
		int32_t mSendIntervalInMilliseconds;
		int32_t mVisitStoreVersion;

		bool mIsCapture;
		bool mIsCaptureCrashes;
		bool mIsCaptureErrors;

		int32_t mMultiplicity;
		int32_t mServerId;

		int64_t mTimestampInMilliseconds;
	};
}

#endif
