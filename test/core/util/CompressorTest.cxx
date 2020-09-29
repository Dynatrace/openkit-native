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

#include "core/util/Compressor.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <vector>

using Compressor_t = base::util::Compressor;

class CompressorTest : public testing::Test
{
};

TEST_F(CompressorTest, gzipCompressHelloWorld)
{
	const char inData[] = "Hello World";
	size_t inDataSize = strlen(inData) + 1;

	std::vector<unsigned char> readBuffer;
	Compressor_t::compressMemory(inData, inDataSize, readBuffer);

	// verify the GZIP magical number
	EXPECT_EQ(readBuffer[0], 0x1F);
	EXPECT_EQ(readBuffer[1], 0x8B);
	EXPECT_EQ(readBuffer[2], 0x08);
}