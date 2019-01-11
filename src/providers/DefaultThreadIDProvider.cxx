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

#include "DefaultThreadIDProvider.h"

#include <thread> 

using namespace providers;

/*
 * std::this_thread::get_id returns a long value which is a hash from the std::thread::id class.
 * The Beacon protocol requires the thread id to be a positive integer value. By using the xor operation
 * between higher and lower 32 bits of the long value we get an integer value. The returned integer
 * can be negative though.
 * Therefore  the most significant bit is forced to '0' by a bitwise-and operation with an integer 
 * where all bits except for the most significant bit are set to '1'.
 */
int32_t DefaultThreadIDProvider::getThreadID()
{
	int64_t hash = std::hash<std::thread::id>()(std::this_thread::get_id());
	return convertNativeThreadIDToPositiveInteger(hash);
}

int32_t DefaultThreadIDProvider::convertNativeThreadIDToPositiveInteger(int64_t nativeThreadID)
{
	uint32_t lowerBits = (uint32_t)nativeThreadID;//cut the higher 32 bits away
	uint32_t higherBits = (uint32_t)(nativeThreadID >> 32);//shift the higher 32 bits to the right
	int32_t threadID = (higherBits ^ lowerBits) & 0x7fffffff; // clear most significant bit
	return threadID;
}