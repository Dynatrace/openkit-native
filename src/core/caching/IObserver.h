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

#ifndef _CORE_CACHING_IOBSERVER_H
#define _CORE_CACHING_IOBSERVER_H

namespace core
{
	namespace caching
	{
		///
		/// The observer interface describes the methods to be implemented by a concrete observer.
		///
		class IObserver
		{
		public:

			///
			/// Destructor
			///
			virtual ~IObserver() = default;

			///
			/// Called to notify/update the registered observers.
			///
			virtual void update() = 0;
		};
	}
}

#endif
