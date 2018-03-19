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

#include <mutex>
#include <condition_variable>

namespace core
{
	namespace util
	{
		class ReadWriteLock
		{
		public:
			ReadWriteLock()
				: shared()
				, readerQ(), writerQ()
				, active_readers(0), waiting_writers(0), active_writers(0)
			{
			}

			void ReadLock()
			{
				std::unique_lock<std::mutex> lk(shared);
				while (waiting_writers != 0)
				{
					readerQ.wait(lk);
				}
				++active_readers;
				lk.unlock();
			}

			void ReadUnlock()
			{
				std::unique_lock<std::mutex> lk(shared);
				--active_readers;
				lk.unlock();
				writerQ.notify_one();
			}

			void WriteLock()
			{
				std::unique_lock<std::mutex> lk(shared);
				++waiting_writers;
				while (active_readers != 0 || active_writers != 0)
				{
					writerQ.wait(lk);
				}
				++active_writers;
				lk.unlock();
			}

			void WriteUnlock()
			{
				std::unique_lock<std::mutex> lk(shared);
				--waiting_writers;
				--active_writers;
				if (waiting_writers > 0)
				{
					writerQ.notify_one();
				}
				else
				{
					readerQ.notify_all();
				}
				lk.unlock();
			}

		private:
			std::mutex              shared;
			std::condition_variable readerQ;
			std::condition_variable writerQ;
			int                     active_readers;
			int                     waiting_writers;
			int                     active_writers;
		};
	}
}

#endif
