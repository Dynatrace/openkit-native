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

#include "Compressor.h"

#include <stdint.h>
#include <zlib.h>
#include "assert.h"

using namespace base::util;

#define WINDOW_BITS   15
#define GZIP_ENCODING 16

void Compressor::compressMemory(const void* inData, size_t inDataSize, std::vector<unsigned char>& outData)
{
	std::vector<uint8_t> buffer;

	const size_t BUFSIZE = 128 * 1024;
	uint8_t tmpBuffer[BUFSIZE];

	z_stream strm;
	strm.zalloc = 0;
	strm.zfree = 0;
	strm.next_in = (Bytef*)inData;
	strm.avail_in = inDataSize;
	strm.next_out = tmpBuffer;
	strm.avail_out = BUFSIZE;

	// Use GZIP with default compresssion
	deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, WINDOW_BITS | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY);

	int32_t res = Z_OK;
	while (strm.avail_in != 0 && res == Z_OK)
	{
		res = deflate(&strm, Z_NO_FLUSH);
		assert(res == Z_OK);
		if (strm.avail_out == 0)
		{
			buffer.insert(buffer.end(), tmpBuffer, tmpBuffer + BUFSIZE);
			strm.next_out = tmpBuffer;
			strm.avail_out = BUFSIZE;
		}
	}

	uint32_t deflateResult = Z_OK;
	do
	{
		if (strm.avail_out == 0)
		{
			buffer.insert(buffer.end(), tmpBuffer, tmpBuffer + BUFSIZE);
			strm.next_out = tmpBuffer;
			strm.avail_out = BUFSIZE;
		}
		deflateResult = deflate(&strm, Z_FINISH);
	} while (deflateResult == Z_OK);

	assert(deflateResult == Z_STREAM_END);
	buffer.insert(buffer.end(), tmpBuffer, tmpBuffer + BUFSIZE - strm.avail_out);
	deflateEnd(&strm);

	outData.swap(buffer);
}

