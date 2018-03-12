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

#ifndef _CORE_UTIL_SYNCHRONIZEDQUEUE_H
#define _CORE_UTIL_SYNCHRONIZEDQUEUE_H

#include <mutex>
#include <vector>
#include <algorithm>
///
/// SynchronizedQueue is an implementation of a data structure that fulfills the following requirements:
/// - has to be thread-safe for access from multiple threads
/// - should be non-blocking for performance reasons
/// - random-delete has to be possible
/// - first-in, first-out
/// - shallow copies should be possible
/// - should be easy to clear
/// As there was no real fit in the C++11 data structures, this is a simple self-made implementation.
/// It's for sure not the best-performing implementation and it could make sense to introduce upper bounds, but it works well enough.
/// @param <T> type of items in the queue instance
///
namespace core
{
	namespace util
	{
		template <class T> class SynchronizedQueue
		{
		public:
			///
			/// Constructor creating an empty list
			///
			SynchronizedQueue()
				: mList()
				, mMutex()
			{
			}

			///
			/// Put an item at the end of the list
			/// @param[in] entry the element to add at the end of the list
			/// @returns @c true if the element could be placed at the end of the list
			///
			bool put(T entry)
			{
				std::lock_guard<std::mutex> lock(mMutex);
				mList.push_back(entry);
				return true;
			}

			///
			/// Return the first element
			/// @returns the first element in the list
			///
			T get()
			{
				std::lock_guard<std::mutex> lock(mMutex);
				T item = mList.front();
				remove(item);
				return item;
			}

			///
			/// Remove a specific item from the queue
			/// @param[in] entry the item to search for
			/// @returns @c true if the element was found in the list and deleted @c false in all other cases
			///
			bool remove(T entry)
			{
				std::lock_guard<std::mutex> lock(mMutex);
				auto found = std::find(mList.begin(), mList.end(), entry);
				if ( found != mList.end())
				{
					mList.erase(found);
					return true;
				}
				return false;
			}

			///
			/// Clear the complete queue
			///
			void clear()
			{
				std::lock_guard<std::mutex> lock(mMutex);
				mList.clear();
			}

			///
			/// Check if queue is empty
			/// @returns @true if queue is empty @false if queue is not empty
			bool isEmpty()
			{
				std::lock_guard<std::mutex> lock(mMutex);
				return mList.empty();
			}
		private:
			/// The queue
			std::vector<T> mList;

			/// mutex for exclusive access to list
			std::mutex mMutex;
		};
	}
}
#endif