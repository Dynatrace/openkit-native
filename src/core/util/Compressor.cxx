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

#include "Compressor.h"

#include <algorithm>

#include <zlib.h>

using namespace base::util;

void Compressor::compressMemory(const void* inData, size_t inDataSize, std::vector<unsigned char>& buffer)
{
	int bufferSize = compressBound(inDataSize); // get buffer size big enough for compression of input data with size in_data_size

	buffer.resize(bufferSize, 0);//init with value 0
	int result = compress(reinterpret_cast<Bytef*>(&(buffer.at(0))), (unsigned long*)&bufferSize, reinterpret_cast<const Bytef*>(inData), inDataSize);
	if (result != Z_OK)
	{
		buffer.clear();

	}
	return;
}
