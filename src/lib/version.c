#include <stdio.h>

int get_version_json(const char *file_path, const char *url)
{
  FILE *fp = fopen(file_path, "wb");
  if (fp == NULL)
  {
    printf("Error creating version.json file.\n");
    return 1;
  }

  int status = curl_get(url, fp);
  fclose(fp);
  return status;
}

int parse_version_json(const char *file_path)
{
}