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

#ifndef _CORE_OBJECTS_NULLSESSION_H
#define _CORE_OBJECTS_NULLSESSION_H

#include "OpenKit/ISession.h"
#include "OpenKit/IRootAction.h"
#include "OpenKit/IWebRequestTracer.h"
#include <OpenKit/json/JsonValue.h>

#include <memory>


namespace core
{
	namespace objects
	{
		///
		/// This class is returned as Session by @ref openkit::IOpenKit::createSession(const char*) when the
		/// @ref openkit::IOpenKit.shutdown() has been called before.
		///
		class NullSession : public openkit::ISession
		{
		public:

			static const std::shared_ptr<NullSession> instance();

			std::shared_ptr<openkit::IRootAction> enterAction(const char* /*actionName*/) override;

			void identifyUser(const char* /*userTag*/) override;

			void reportCrash(const char* /*errorName*/, const char* /*reason*/, const char* /*stacktrace*/) override;

			std::shared_ptr<openkit::IWebRequestTracer> traceWebRequest(const char* /*url*/) override;

			void end() override;

			void sendBizEvent(const char* /*type*/, const openkit::json::JsonObjectValue::JsonObjectMapPtr /*attributes*/) override;

			void sendEvent(const char* /*name*/, const openkit::json::JsonObjectValue::JsonObjectMapPtr /*attributes*/) override;
		};
	}
}

#endif
