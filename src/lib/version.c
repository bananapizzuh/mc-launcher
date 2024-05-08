#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "util.h"

int get_version_json(const char *file_path, const char *url)
{
  FILE *fp = fopen(file_path, "w");
  if (fp == NULL)
  {
    printf("Error creating version.json file.\n");
    return 1;
  }

  int status = curl_get(url, fp);
  fclose(fp);
  return status;
}

int parse_version_json(const char *file_path, const char *asset_path)
{
  FILE *fp;
  char *buffer;
  long file_size;
  size_t result;
  cJSON *json;

  fp = fopen(file_path, "rb");

  if (fp == NULL)
  {
    printf("Error opening manifest.json file.\n");
    return 1;
  }

  fseek(fp, 0, SEEK_END);
  file_size = ftell(fp);
  rewind(fp);

  buffer = (char *)malloc(sizeof(char) * file_size);
  if (buffer == NULL)
  {
    fprintf(stderr, "Memory error.");
    fclose(fp);
    return 1;
  }

  result = fread(buffer, 1, file_size, fp);
  if (result != file_size)
  {
    fprintf(stderr, "Reading error.");
    free(buffer);
    fclose(fp);
    return 1;
  }

  fclose(fp);

  json = cJSON_Parse(buffer);
  if (json == NULL)
  {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL)
    {
      printf("Error: %s\n", error_ptr);
    }
    cJSON_Delete(json);
    return 1;
  }

  cJSON *libraries = cJSON_GetObjectItemCaseSensitive(json, "libraries");
  if (cJSON_IsArray(libraries))
  {
    cJSON *library = NULL;
    cJSON_ArrayForEach(library, libraries)
    {

      cJSON *downloads = cJSON_GetObjectItemCaseSensitive(library, "downloads");
      if (cJSON_IsObject(downloads))
      {
        cJSON *artifact = cJSON_GetObjectItemCaseSensitive(downloads, "artifact");
        if (cJSON_IsObject(artifact))
        {
          char *path;
          char *name;
          char *url;
          cJSON *path_json = cJSON_GetObjectItemCaseSensitive(artifact, "path");
          if (cJSON_IsString(path_json) && (path_json->valuestring != NULL))
          {
            path = malloc(strlen(asset_path) + strlen(dirname(path_json->valuestring)) + 1);
            sprintf(path, "%s%s", asset_path, dirname(path_json->valuestring));

            struct stat st = {0};
            if (stat(path, &st) == -1)
            {
              printf("path: %s\n", path);
              mkdir_p(path, 0700);
            }

            name = basename(path_json->valuestring);
            strcat(name, ".jar");
          }

          cJSON *url_json = cJSON_GetObjectItemCaseSensitive(artifact, "url");
          if (cJSON_IsString(url_json) && (url_json->valuestring != NULL))
          {
            url = url_json->valuestring;
          }

          char *final_path = malloc(strlen(path) + strlen(name) + 1);
          sprintf(final_path, "%s%s", path, name);
          FILE *lib = fopen(final_path, "wb");
          curl_get(url, lib);
        }
      }
    }
  }
}