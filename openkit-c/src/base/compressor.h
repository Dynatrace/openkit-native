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

#ifndef _COMPRESSOR_H
#define _COMPRESSOR_H

#include "zlib.h"
#include "stddef.h"

typedef struct {
	size_t length;
	unsigned char* data;
} binaryData;

void compress_memory(const void *in_data, size_t in_data_size, binaryData* out_data);
#endif