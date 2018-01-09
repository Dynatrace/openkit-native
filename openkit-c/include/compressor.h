#include "zlib.h"
#include "stddef.h"

typedef struct {
	long length;
	unsigned char* data;
} binaryData;

void compress_memory(const void *in_data, size_t in_data_size, binaryData* out_data);