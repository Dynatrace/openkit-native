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

#ifndef _PROVIDERS_DEFAULTSESSIONIDPROVIDER_H
#define _PROVIDERS_DEFAULTSESSIONIDPROVIDER_H

#include "ISessionIDProvider.h"
#include <stdint.h>
#include <mutex>

namespace providers
{
	///
	/// Default implementation of @ref providers::ISessionIDProvider interface
	///
	class DefaultSessionIDProvider : public ISessionIDProvider
	{
	public:

		///
		/// Default constructor
		///
		DefaultSessionIDProvider();

		///
		/// Construtor with inital offset
		///
		DefaultSessionIDProvider(int32_t initialOffset);

		///
		/// Destructor
		///
		virtual ~DefaultSessionIDProvider() {}

		///
		/// Provide the next sessionID
		/// All positive integers greater than 0 can be used as sessionID
		/// @returns the id that will be used for the next session
		///
		virtual int32_t getNextSessionID();
	private:

		/// remember last session number - initialized with random offset
		int32_t mLastSessionNumber;

		/// mutex for exclusive access to getNextSessionID
		std::mutex mNextIDMutex;
	};
}
#endif
