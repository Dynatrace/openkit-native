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

#ifndef _PROVIDERS_DEFAULTTHREADIDPROVIDER_H
#define _PROVIDERS_DEFAULTTHREADIDPROVIDER_H

#include "IThreadIDProvider.h"

namespace providers
{

	///
	/// Simple ThreadIDProvider implementation for getting the current thread ID.
	///
	class DefaultThreadIDProvider : public IThreadIDProvider
	{
	public:
		///
		/// Provide the current thread ID
		/// @returns the current thread ID
		///
		int32_t getThreadID() override;

		///
		/// Convert a native thread id to a positive integer required for the Beacon protocol
		/// @param[in] nativeThreadID the native thread ID returned by std::this_thread::get_id
		/// @returns a positive integer calculated from the native ID
		///
		static int32_t convertNativeThreadIDToPositiveInteger(int64_t nativeThreadID);
	};
}

#endif