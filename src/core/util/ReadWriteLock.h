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

#ifndef _CORE_UTIL_READWRITELOCK_H
#define _CORE_UTIL_READWRITELOCK_H

#if defined(_WIN32) || defined(WIN32) 
#include <Windows.h>
#else
#include <pthread.h>
#endif

namespace core
{
	namespace util
	{
		class ReadWriteLock
		{
		public:
#if defined(_WIN32) || defined(WIN32) 
			ReadWriteLock()
				: mSrwLock()
			{
				InitializeSRWLock(&mSrwLock);
			}

			void ReadLock()
			{
				AcquireSRWLockShared(&mSrwLock);
			}

			void ReadUnlock()
			{
				ReleaseSRWLockShared(&mSrwLock);
			}

			void WriteLock()
			{
				AcquireSRWLockExclusive(&mSrwLock);
			}

			void WriteUnlock()
			{
				ReleaseSRWLockExclusive(&mSrwLock);
			}

		private:
			SRWLOCK mSrwLock;
#else
			ReadWriteLock()
				: mLock_rw(PTHREAD_RWLOCK_INITIALIZER)
			{
			}

			void ReadLock()
			{
				pthread_rwlock_rdlock(&mLock_rw);
			}

			void ReadUnlock()
			{
				pthread_rwlock_unlock(&mLock_rw);
			}

			void WriteLock()
			{
				pthread_rwlock_wrlock(&mLock_rw);
			}

			void WriteUnlock()
			{
				pthread_rwlock_unlock(&mLock_rw);
			}

		private:
			pthread_rwlock_t mLock_rw;
#endif
		};
	}
}

#endif
