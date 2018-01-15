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

#include "compressor.h"

#include "memory.h"

void compress_memory(const void *in_data, size_t in_data_size, binaryData* out_data)
{
	if (out_data == NULL)
	{
		return;
	}

	int bufferSize = compressBound(in_data_size); // get buffer size big enough for compression of input data with size in_data_size

	out_data->data = (unsigned char*)memory_malloc(bufferSize);
	out_data->length = (unsigned long)bufferSize;

	int result = compress(out_data->data, (unsigned long*)&bufferSize, in_data, in_data_size);
	if (result != Z_OK)
	{
		memory_free(out_data->data);
		out_data->data = NULL;
		out_data->length = -1;
	}
	return;
	//ZEXTERN int ZEXPORT compress OF((Bytef *dest, uLongf *destLen,
	//	const Bytef *source, uLong sourceLen));
}
