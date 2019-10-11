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

#ifndef _TEST_API_TYPES_H
#define _TEST_API_TYPES_H

#include "OpenKit/AppMonOpenKitBuilder.h"
#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "OpenKit/DynatraceOpenKitBuilder.h"
#include "OpenKit/IAction.h"
#include "OpenKit/IRootAction.h"
#include "OpenKit/ISession.h"
#include "OpenKit/ISSLTrustManager.h"
#include "OpenKit/IWebRequestTracer.h"
#include "OpenKit/LogLevel.h"

#include <memory>

namespace test
{
	namespace types
	{
		using AppMonOpenKitBuilder_t = openkit::AppMonOpenKitBuilder;
		using AppMonOpenKitBuilder_sp = std::shared_ptr<AppMonOpenKitBuilder_t>;

		using CrashReportingLevel_t = openkit::CrashReportingLevel;
		using DataCollectionLevel_t = openkit::DataCollectionLevel;

		using DynatraceOpenKitBuilder_t = openkit::DynatraceOpenKitBuilder;
		using DynatraceOpenKitBuilder_sp = std::shared_ptr<DynatraceOpenKitBuilder_t>;

		using IAction_t = openkit::IAction;
		using IAction_sp = std::shared_ptr<IAction_t>;

		using ILogger_t = openkit::ILogger;
		using ILogger_sp = std::shared_ptr<ILogger_t>;

		using IOpenKit_t = openkit::IOpenKit;
		using IOpenkit_sp = std::shared_ptr<IOpenKit_t>;

		using IRootAction_t = openkit::IRootAction;
		using IRootAction_sp = std::shared_ptr<IRootAction_t>;

		using ISession_t = openkit::ISession;
		using ISession_sp = std::shared_ptr<ISession_t>;

		using ISslTrustManager_t = openkit::ISSLTrustManager;
		using ISslTrustManager_sp = std::shared_ptr<ISslTrustManager_t>;

		using IWebRequestTracer_t = openkit::IWebRequestTracer;
		using IWebRequestTracer_sp = std::shared_ptr<IWebRequestTracer_t>;

		using LogLevel_t = openkit::LogLevel;
	}
}

#endif
