/**
* Copyright 2018 Dynatrace LLC
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
#ifndef _UTIL_DEFAULTLOGGER_H
#define _UTIL_DEFAULTLOGGER_H

#include "api/ILogger.h"

#include <string>
#include <sstream>

namespace core
{
	namespace util
	{
		class DefaultLogger : public api::ILogger
		{
		public:
			///
			/// Default constructor
			///
			DefaultLogger();

			///
			/// Constructor
			/// param[in] versbose a flag which is @c true to enable INFO and DEBUG traces
			///
			DefaultLogger(bool verbose);

			///
			/// Constructor to let the default logger write to the provided stream. Intended for unit testing.
			/// param[in] stream the stream where the default logger shall write to
			/// param[in] versbose a flag which is @c true to enable INFO and DEBUG traces
			///
			DefaultLogger(std::ostream &stream, bool verbose);

			///
			/// Destructor
			///
			virtual ~DefaultLogger() {}

			virtual void error(const char* format, ...) override;

			virtual void warning(const char* format, ...) override;

			virtual void info(const char* format, ...) override;

			virtual void debug(const char* format, ...) override;

			virtual bool isErrorEnabled() const override;

			virtual bool isWarningEnabled() const override;

			virtual bool isInfoEnabled() const override;

			virtual bool isDebugEnabled() const override;

		private:
			///
			/// Does the actual logging to the @ref mStream.
			/// param[in] level the log level is added to the trace
			/// param[in] format the format string in a printf style
			/// param[in] args the list of variable arguments to be passed to printf
			///
			void doLog(const char* level, const char* format, va_list args);

			/// The stream to be logged to
			std::ostream &mStream;

			/// Flag to enable INFO and DEBUG traces
			bool mVerbose;
		};
	}
}

#endif
