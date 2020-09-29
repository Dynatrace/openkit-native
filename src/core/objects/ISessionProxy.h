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

#ifndef _CORE_OBJECTS_ISESSIONPROXY_H
#define _CORE_OBJECTS_ISESSIONPROXY_H

#include "OpenKit/ISession.h"

#include "core/configuration/IServerConfiguration.h"

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
		};
	}
}

#endif
