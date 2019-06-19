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

#ifndef _CORE_UTIL_SCOPEDREADLOCK_H
#define _CORE_UTIL_SCOPEDREADLOCK_H

#include "ReadWriteLock.h"

namespace core
{
	namespace util
	{
		///
		/// The scoped READ lock is a wrapper class wrapping the @ref ReadWriteLock to achieve a scope-based life time.
		/// If the scope of the scoped lock object is left then the wrapped lock is unlocked (following the RAII idiom).
		///
		class ScopedReadLock
		{
		public:
			///
			/// Constructor
			/// @param[in] lk the @ref ReadWriteLock to be wrapped
			///
			ScopedReadLock(ReadWriteLock& lk)
				: mLock(&lk)
				, mOwns(false)
			{
				mLock->ReadLock();
				mOwns = true;
			}

			///
			/// Destructor, unlocking the wrapped @ref ReadWriteLock
			///
			~ScopedReadLock()
			{
				if (mOwns)
				{
					mLock->ReadUnlock();
				}
			}

			///
			/// Delete the copy constructor
			///
			ScopedReadLock(const ScopedReadLock&) = delete;

			///
			/// Delete the assignment operator
			///
			ScopedReadLock& operator=(const ScopedReadLock&) = delete;

			///
			/// To lock the wrapped @ref ReadWriteLock
			///
			void lock()
			{
				if (!mOwns)
				{
					mLock->ReadLock();
					mOwns = true;
				}
			}

			///
			/// To unlock the wrapped @ref ReadWriteLock
			///
			void unlock()
			{
				if (mOwns)
				{
					mLock->ReadUnlock();
					mOwns = false;
				}
			}

		private:
			/// The wrapped @ref ReadWriteLock
			ReadWriteLock * mLock;

			/// flag indicating @c true if the wrapped @ref ReadWriteLock is locked, @c false otherwise
			bool mOwns;
		};
	}
}

#endif
