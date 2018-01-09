#include "curl/curl.h"

#include "compressor.h"
#include "memory.h"

const char* ascii_json_data =
"{"
"	\"data\": {"
"	\"name\": \"Name\","
"		\"number\" : 4,"
"		\"text\" : \"Some data\""
"	}"
"}";

struct BinaryDataWriter {
	const unsigned char *readptr;
	size_t sizeleft;
};

void make_get_request(CURL* curl)
{
	CURLcode res;

	/* First set the URL that is about to receive our POST. This URL can
	just as well be a https:// URL if that is what should receive the
	data. */
	curl_easy_setopt(curl, CURLOPT_URL, "https://postman-echo.com/GET?foo1=bar1&foo2=bar2");
	/* Now specify the POST data */

	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl);
	/* Check for errors */
	if (res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));

	printf("\n\tReturned with result code %d\n", res);
}

void make_post_request_text(CURL* curl)
{
	CURLcode res;

	/* First set the URL that is about to receive our POST. This URL can
	just as well be a https:// URL if that is what should receive the
	data. */
	curl_easy_setopt(curl, CURLOPT_URL, "https://postman-echo.com/POST");
	/* Now specify the POST data */
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ascii_json_data);

	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl);
	/* Check for errors */
	if (res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));

	printf("\n\tReturned with result code %d\n", res);
}

static size_t read_callback(void *dest, size_t size, size_t nmemb, void *userp)
{
	struct BinaryDataWriter *binary = (struct BinaryDataWriter *)userp;
	size_t buffer_size = size * nmemb;

	if (binary->sizeleft) {
		/* copy as much as possible from the source to the destination */
		size_t copy_this_much = binary->sizeleft;
		if (copy_this_much > buffer_size)
			copy_this_much = buffer_size;
		memcpy(dest, binary->readptr, copy_this_much);

		binary->readptr += copy_this_much;
		binary->sizeleft -= copy_this_much;
		return copy_this_much; /* we copied this many bytes */
	}

	return 0; /* no more data left to deliver */
}

void make_post_request_compressed(CURL* curl)
{
	CURLcode res;

	binaryData compressedBuffer;
	compress_memory(&ascii_json_data[0], strlen(ascii_json_data), &compressedBuffer);

	struct BinaryDataWriter writer;
	writer.readptr = compressedBuffer.data;
	writer.sizeleft = compressedBuffer.length;
	/* First set the URL that is about to receive our POST. This URL can
	just as well be a https:// URL if that is what should receive the
	data. */

	curl_easy_setopt(curl, CURLOPT_URL, "https://localhost:9999/mbeacon/1?type=m&srvid=5&app=56236c21-40b1-498f-aed9-65bbb75580d2&va=7.0.0000&pt=1&tt=okc");
	
	/* Disable the SSL host name checks*/
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	
	/* allow for compressed data*/
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

	/* Now specify the POST data */
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	/*we want to use our own read function */
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
	curl_easy_setopt(curl, CURLOPT_READDATA, &writer);

	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl);
	/* Check for errors */
	if (res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));

	memory_free(compressedBuffer.data);
	compressedBuffer.length = -1;

	printf("\n\tReturned with result code %d\n", res);
}

int main(int argc, char* *argv)
{
	CURL *curl;


	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		
		make_get_request(curl);
		make_post_request_text(curl);
		make_post_request_compressed(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return 0;
}
