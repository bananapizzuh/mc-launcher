#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <curl/curl.h>

#define PATH_MAX_STRING_SIZE 256

char *url_encode(const char *str)
{
  CURL *curl = curl_easy_init();
  if (curl)
  {
    char *output = curl_easy_escape(curl, str, strlen(str));
    if (output)
    {
      return output;
    }
    curl_easy_cleanup(curl);
  }
  return NULL;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  return fwrite(ptr, size, nmemb, stream);
}

int curl_get(const char *url, FILE *output)
{
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
      return res;
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return 0;
}

/* recursive mkdir https://gist.github.com/ChisholmKyle/0cbedcd3e64132243a39 */
int mkdir_p(const char *dir, const mode_t mode)
{
  char tmp[PATH_MAX_STRING_SIZE];
  char *p = NULL;
  struct stat sb;
  size_t len;

  /* copy path */
  len = strnlen(dir, PATH_MAX_STRING_SIZE);
  if (len == 0 || len == PATH_MAX_STRING_SIZE)
  {
    return -1;
  }
  memcpy(tmp, dir, len);
  tmp[len] = '\0';

  /* remove trailing slash */
  if (tmp[len - 1] == '/')
  {
    tmp[len - 1] = '\0';
  }

  /* check if path exists and is a directory */
  if (stat(tmp, &sb) == 0)
  {
    if (S_ISDIR(sb.st_mode))
    {
      return 0;
    }
  }

  /* recursive mkdir */
  for (p = tmp + 1; *p; p++)
  {
    if (*p == '/')
    {
      *p = 0;
      /* test path */
      if (stat(tmp, &sb) != 0)
      {
        /* path does not exist - create directory */
        if (mkdir(tmp, mode) < 0)
        {
          return -1;
        }
      }
      else if (!S_ISDIR(sb.st_mode))
      {
        /* not a directory */
        return -1;
      }
      *p = '/';
    }
  }
  /* test path */
  if (stat(tmp, &sb) != 0)
  {
    /* path does not exist - create directory */
    if (mkdir(tmp, mode) < 0)
    {
      return -1;
    }
  }
  else if (!S_ISDIR(sb.st_mode))
  {
    /* not a directory */
    return -1;
  }
  return 0;
}

/* str split https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c */
char **str_split(char *a_str, const char a_delim, size_t *len)
{
  char **result = 0;
  size_t count = 0;
  char *tmp = a_str;
  char *last_comma = 0;
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

  /* Count how many elements will be extracted. */
  while (*tmp)
  {
    if (a_delim == *tmp)
    {
      count++;
      last_comma = tmp;
    }
    tmp++;
  }

  /* Add space for trailing token. */
  count += last_comma < (a_str + strlen(a_str) - 1);

  /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
  count++;

  result = malloc(sizeof(char *) * count);
  *len = sizeof(char *) * count;

  if (result)
  {
    size_t idx = 0;
    char *token = strtok(a_str, delim);

    while (token)
    {
      assert(idx < count);
      *(result + idx++) = strdup(token);
      token = strtok(0, delim);
    }
    assert(idx == count - 1);
    *(result + idx) = 0;
  }

  return result;
}
