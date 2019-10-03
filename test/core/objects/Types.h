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

#ifndef _TEST_CORE_OBJECTS_TYPES_H
#define _TEST_CORE_OBJECTS_TYPES_H

#include "core/objects/Action.h"
#include "core/objects/ActionCommonImpl.h"
#include "core/objects/NullAction.h"
#include "core/objects/NullRootAction.h"
#include "core/objects/NullSession.h"
#include "core/objects/NullWebRequestTracer.h"
#include "core/objects/OpenKit.h"
#include "core/objects/RootAction.h"
#include "core/objects/Session.h"
#include "core/objects/WebRequestTracer.h"

namespace test
{
	namespace types
	{
		using Action_t = core::objects::Action;
		using Action_sp = std::shared_ptr<Action_t>;

		using ActionCommonImpl_t = core::objects::ActionCommonImpl;
		using ActionCommonImpl_sp = std::shared_ptr<ActionCommonImpl_t>;

		using NullAction_t = core::objects::NullAction;
		using NullAction_sp = std::shared_ptr<NullAction_t>;

		using NullRootAction_t = core::objects::NullRootAction;
		using NullRootAction_sp = std::shared_ptr<NullRootAction_t>;

		using NullSession_t = core::objects::NullSession;
		using NullSession_sp = std::shared_ptr<NullSession_t>;

		using NullWebRequestTracer_t = core::objects::NullWebRequestTracer;
		using NullWebRequestTracer_sp = std::shared_ptr<NullWebRequestTracer_t>;

		using OpenKit_t = core::objects::OpenKit;
		using OpenKit_sp = std::shared_ptr<OpenKit_t>;

		using RootAction_t = core::objects::RootAction;
		using RootAction_sp = std::shared_ptr<RootAction_t>;

		using Session_t = core::objects::Session;
		using Session_sp = std::shared_ptr<Session_t>;

		using WebRequestTracer_t = core::objects::WebRequestTracer;
		using WebRequestTracer_sp = std::shared_ptr<WebRequestTracer_t>;
	}
}

#endif
