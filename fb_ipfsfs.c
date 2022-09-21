#include "filebench.h"

#include "fb_ipfsfs.h"
#include <curl/curl.h> /* libcurl library */

/*
 * Types and functions to write HTTP response into memory (MemoryStruct).
 */
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
  // mem->memory is already an allocated buffer by filebench
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

/*
 * Helper to do a libcurl write.
 */
int fb_ipfs_write(char *path, caddr_t iobuf, fbint_t iosize, off64_t offset)
{
  // libcurl stuff
  CURL *curl;
  CURLcode response;

  // form and file field for libcurl request
  curl_mime *form = NULL;
  curl_mimepart *field = NULL;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  if (curl)
  {
    // Create the form
    form = curl_mime_init(curl);

    // Fill in the file upload field
    field = curl_mime_addpart(form);
    curl_mime_name(field, "file");
    curl_mime_data(field, iobuf, iosize);

    char *url = (char *)malloc(150 * sizeof(char));
    if (offset > 0)
    {
      size_t needed = snprintf(NULL, 0, "http://localhost:5001/api/v0/files/write?arg=%s&create=true&parents=true&offset=%zu", path, offset);
      url = malloc(needed + 1);
      sprintf(url, "http://localhost:5001/api/v0/files/write?arg=%s&create=true&parents=true&offset=%zu", path, offset);
    }
    else
    {
      size_t needed = snprintf(NULL, 0, "http://localhost:5001/api/v0/files/write?arg=%s&create=true&parents=true", path);
      url = malloc(needed + 1);
      sprintf(url, "http://localhost:5001/api/v0/files/write?arg=%s&create=true&parents=true", path);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

    response = curl_easy_perform(curl);

    // check if response completes + is written to memory
    if (response != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() write failed for %s: %s\n", path, curl_easy_strerror(response));
      return (FILEBENCH_ERROR);
    }

    // cleanup
    curl_easy_cleanup(curl);
    curl_mime_free(form);
    free(url);
  }

  curl_global_cleanup();

  return iosize;
}

/*
 * Helper to do a libcurl read.
 */
int fb_ipfs_read(char *path, caddr_t iobuf, fbint_t iosize, off64_t fileoffset)
{
  // libcurl stuff
  CURL *curl;
  CURLcode response;
  struct MemoryStruct chunk = {iobuf, 0};

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  if (curl)
  {
    char *url;
    if (fileoffset > 0)
    {
      size_t needed = snprintf(NULL, 0, "http://localhost:5001/api/v0/files/read?arg=%s&count=%ld&offset=%zu", path, iosize, fileoffset);
      url = malloc(needed + 1);
      sprintf(url, "http://localhost:5001/api/v0/files/read?arg=%s&count=%ld&offset=%zu", path, iosize, fileoffset);
    }
    else
    {
      size_t needed = snprintf(NULL, 0, "http://localhost:5001/api/v0/files/read?arg=%s&count=%ld", path, iosize);
      url = malloc(needed + 1);
      sprintf(url, "http://localhost:5001/api/v0/files/read?arg=%s&count=%ld", path, iosize);
    }

    // send all data to this function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    // we pass our 'chunk' struct to the callback function
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    // specify the POST data which is empty for this endpoint
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

    response = curl_easy_perform(curl);

    // check if response completes + is written to memory
    if (response != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() reading failed for %s: %s\n", path, curl_easy_strerror(response));
      return -1;
    }

    // cleanup
    curl_easy_cleanup(curl);
    free(url);
  }

  curl_global_cleanup();

  return 0;
}

int fb_ipfs_generic_post(char *url)
{
  // libcurl stuff
  CURL *curl;
  CURLcode response;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    // specify the POST data which is empty for this endpoint
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

    response = curl_easy_perform(curl);

    // check if response completes + is written to memory
    if (response != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() renaming failed: %s\n", curl_easy_strerror(response));
      return (FILEBENCH_ERROR);
    }

    // cleanup
    curl_easy_cleanup(curl);
    free(url);
  }

  curl_global_cleanup();
  return (FILEBENCH_OK);
}
