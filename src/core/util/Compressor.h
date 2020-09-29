/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _CORE_UTIL_COMPRESSOR_H
#define _CORE_UTIL_COMPRESSOR_H

#include <vector>
#include <cstddef>

namespace base
{
	namespace util
	{
		///
		/// Utility class to compress data with zlib
		///
		class Compressor
		{
		public:

			///
			/// Compress block of memory at in_data with a length of @c inDataSize bytes 
			/// @param[in] inData pointer to the incoming data
			/// @param[in] inDataSize size of data behind the pointer (measured in bytes)
			/// @param[out] out_data binary_data struct passed as reference that will contain the compressed data.
			///
			static void compressMemory(const void *inData, size_t inDataSize, std::vector<unsigned char>& out_data);
		};
	}
	
}
#endif