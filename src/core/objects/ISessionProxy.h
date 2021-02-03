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

#ifndef _CORE_OBJECTS_ISESSIONPROXY_H
#define _CORE_OBJECTS_ISESSIONPROXY_H

#include "OpenKit/ISession.h"

#include "core/configuration/IServerConfiguration.h"

#include <cstdint>
#include <memory>

namespace core
{
	namespace objects
	{
		///
		/// Implements a surrogate for a ISession to perform transparent session splitting after a
		/// configured number of top level events or after the expiration of session duration or idle timeout.
		///
		class ISessionProxy : public openkit::ISession
		{
		public:

			///
			/// Indicates whether this session proxy was finished or is still open.
			///
			virtual bool isFinished() = 0;

			///
			/// Callback method which is to be invoked when the server configuration on a session is updated.
			///
			/// @param serverConfig the updated server configuration
			///
			virtual void onServerConfigurationUpdate(std::shared_ptr<core::configuration::IServerConfiguration> serverConfig) = 0;

			///
			/// Will end the current active session and start a new one but only if the following conditions are met:
			/// - this session proxy is not ISessionProxy::isFinished()
			/// - session splitting by idle timeout is enabled and the current session was idle for longer than the
			///   configured timeout.
			/// - session splitting by maximum session duration is enabled and the session was open for longer than the
			///   maximum configured session duration.
			/// 
			/// @return the time when the session might be split next. This can either be the time when the maximum duration
			///         is reached or the time when the idle timeout expires. In case the session proxy is finished, @c -1
			///         is returned.
			///
			virtual int64_t splitSessionByTime() = 0;
		};
	}
}

#endif
