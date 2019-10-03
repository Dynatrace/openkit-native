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

#ifndef _TEST_CORE_UTIL_TYPES_H
#define _TEST_CORE_UTIL_TYPES_H

#include "core/util/Compressor.h"
#include "core/util/CountDownLatch.h"
#include "core/util/CyclicBarrier.h"
#include "core/util/DefaultLogger.h"
#include "core/util/InetAddressValidator.h"
#include "core/util/StringUtil.h"
#include "core/util/SynchronizedQueue.h"
#include "core/util/URLEncoding.h"

#include <memory>

namespace test { namespace types
{
	using Compressor_t = base::util::Compressor;

	using CountDownLatch_t = core::util::CountDownLatch;
	using CountDownLatch_sp = std::shared_ptr<CountDownLatch_t>;

	using CyclicBarrier_t = core::util::CyclicBarrier;
	using CyclicBarrier_sp = std::shared_ptr<CyclicBarrier_t>;

	using DefaultLogger_t = core::util::DefaultLogger;
	using DefaultLogger_sp = std::shared_ptr<DefaultLogger_t>;

	using InetAddressValidator_t = core::util::InetAddressValidator;

	using StringUtil_t = core::util::StringUtil;

	template <typename T>
	using SynchronizedQueue_t = core::util::SynchronizedQueue<T>;

	using UrlEnconding_t = core::util::URLEncoding;
}}

#endif
