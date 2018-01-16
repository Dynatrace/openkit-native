#include "curl/curl.h"

#include <stdint.h>
#include <string.h>

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

struct compressed_data_writer {
	const unsigned char *readptr;
	size_t sizeleft;
};

int32_t make_get_request(CURL* curl)
{
	if (curl != NULL)
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
		return 0;
	}
	return 1;
}

int32_t make_post_request_text(CURL* curl)
{
	if (curl != NULL)
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
		return 0;
	}
	return 1;
}

static size_t read_callback(void *dest, size_t size, size_t nmemb, void *userp)
{
	struct compressed_data_writer *binary = (struct compressed_data_writer *)userp;
	if (binary != NULL)
	{
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
	}
	return 0; /* no more data left to deliver */
}

int32_t make_post_request_compressed(CURL* curl)
{
	if (curl != NULL)
	{
		CURLcode res;

		compressed_data compressed_buffer;
		compress_memory(&ascii_json_data[0], strlen(ascii_json_data), &compressed_buffer);

		struct compressed_data_writer writer;
		writer.readptr = compressed_buffer.data;
		writer.sizeleft = compressed_buffer.length;
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

		memory_free(compressed_buffer.data);
		compressed_buffer.length = -1;

		printf("\n\tReturned with result code %d\n", res);
		return 0;
	}
	return 1;
}

int main(int argc, char* *argv)
{
	CURL *curl;


	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	int32_t result = 0;
	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		
		result = make_get_request(curl);
		result += make_post_request_text(curl);
		result += make_post_request_compressed(curl);
			/* always cleanup */
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return result;
}
