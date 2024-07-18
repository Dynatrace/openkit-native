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

#include "NullWebRequestTracer.h"

using namespace core::objects;

const std::shared_ptr<NullWebRequestTracer> NullWebRequestTracer::instance()
{
	static const auto instance = std::make_shared<NullWebRequestTracer>();

	return instance;
}

const char* NullWebRequestTracer::getTag() const
{
	return emptyString;
}

std::shared_ptr<openkit::IWebRequestTracer> NullWebRequestTracer::setBytesSent(int32_t /*bytesSent*/)
{
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> NullWebRequestTracer::setBytesReceived(int32_t /*bytesReceived*/)
{
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> NullWebRequestTracer::setBytesSent(int64_t /*bytesSent*/)
{
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> NullWebRequestTracer::setBytesReceived(int64_t /*bytesReceived*/)
{
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> NullWebRequestTracer::start()
{
	return shared_from_this();
}

void NullWebRequestTracer::stop(int32_t /*responseCode*/)
{
	// nothing, NullObject pattern
}