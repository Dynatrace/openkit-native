#include "compressor.h"

#include "memory.h"

void compress_memory(const void *in_data, size_t in_data_size, binaryData* out_data)
{
	if (out_data == NULL)
	{
		return;
	}

	int bufferSize = compressBound(in_data_size); // get buffer size big enough for compression of input data with size in_data_size

	out_data->data = (char*)memory_malloc(bufferSize);
	out_data->length = (long)bufferSize;

	int result = compress(out_data->data, (long*)&bufferSize, in_data, in_data_size);
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
