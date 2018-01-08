#include "curl/curl.h"


const char* ascii_json_data =
"{"
"	\"data\": {"
"	\"name\": \"Name\","
"		\"number\" : 4,"
"		\"text\" : \"Some data\""
"	}"
"}";


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

	char errorBuffer[100];

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
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return 0;
}
