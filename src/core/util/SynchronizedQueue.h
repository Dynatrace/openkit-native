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

#ifndef _CORE_UTIL_SYNCHRONIZEDQUEUE_H
#define _CORE_UTIL_SYNCHRONIZEDQUEUE_H

#include <mutex>
#include <list>
#include <algorithm>
#include <vector>

namespace core
{
	namespace util
	{
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
		/// @param T type of items in the queue instance.
		///
		/// Note elements which are added need to overload the '==' operator
		///
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
			void put(const T& entry)
			{
				std::lock_guard<std::mutex> lock(mMutex);
				mList.push_back(entry);
			}

			///
			/// Return the first element
			/// @returns the first element in the list
			///
			T get()
			{
				static T defaultItem;

				std::lock_guard<std::mutex> lock(mMutex);
				if (!mList.empty())
				{
					T element = *mList.begin();
					mList.pop_front();
					return element;
				}
				return defaultItem;
			}

			///
			/// Remove a specific item from the queue
			/// @param[in] entry the item to search for
			/// @returns @c true if the element was found in the list and deleted @c false in all other cases
			///
			bool remove(const T& entry)
			{
				std::lock_guard<std::mutex> lock(mMutex);
				auto iterator = find_first(entry);
				if (iterator == mList.end())
				{
					return false;
				}

				mList.erase(iterator);
				return true;
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
			/// @returns @c true if queue is empty @c false if queue is not empty
			////
			bool isEmpty() const
			{
				std::lock_guard<std::mutex> lock(mMutex);
				return mList.empty();
			}

			///
			/// Returns a shallow copy of the list elements.
			/// @returns a std::vector created from the list
			///
			std::vector<T> toStdVector()
			{
				std::lock_guard<std::mutex> lock(mMutex);
				return std::vector<T>(mList.begin(), mList.end());
			}

		private:
			typedef typename std::list<T>::iterator queue_iterator;

			///
			/// find first occurence of a given element
			/// @param[in] value the element to search for
			/// @returns if the element was found an iterator to the element is returned
			///          else the iterator to list.end() is returned
			///
			queue_iterator find_first(const T& value)
			{
				for (auto it = mList.begin(); it != mList.end(); it++)
				{
					if (*it == value)
					{
						return it;
					}
				}
				return mList.end();
			}

			/// The queue is emulated via linked list
			std::list<T> mList;

			/// mutex for exclusive access to list
			mutable std::mutex mMutex;
		};
	}
}
#endif
