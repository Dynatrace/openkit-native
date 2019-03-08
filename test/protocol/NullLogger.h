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

#ifndef _TEST_PROTOCOL_NULLLOGGER_H
#define _TEST_PROTOCOL_NULLLOGGER_H

#include <OpenKit/ILogger.h>

class NullLogger : public openkit::ILogger
{
public:

	virtual ~NullLogger() {}

	virtual void log(openkit::LogLevel /*logLevel*/, const char* /*format*/, ...) override {}

	virtual void error(const char* /*format*/, ...) override {}

	virtual void warning(const char* /*format*/, ...) override {}

	virtual void info(const char* /*format*/, ...) override {}

	virtual void debug(const char* /*format*/, ...) override {}

	virtual bool isErrorEnabled() const override { return true; }

	virtual bool isWarningEnabled() const override { return true; }

	virtual bool isInfoEnabled() const override { return true; }

	virtual bool isDebugEnabled() const override { return true; }
};

#endif
