#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "util.h"

int parse_asset_index(const char *file_path, const char *obj_path)
{
  FILE *fp;
  char *buffer;
  long file_size;
  size_t result;
  cJSON *json;

  fp = fopen(file_path, "rb");

  if (fp == NULL)
  {
    printf("Error opening asset index file.\n");
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

  json = cJSON_Parse(buffer);
  if (json == NULL)
  {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL)
    {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    cJSON_Delete(json);
    free(buffer);
    fclose(fp);
    return 1;
  }

  cJSON *objects = cJSON_GetObjectItem(json, "objects");
  cJSON *object;
  cJSON_ArrayForEach(object, objects)
  {
    cJSON *hash = cJSON_GetObjectItem(object, "hash");
    if (cJSON_IsString(hash) && (hash->valuestring != NULL))
    {
      char *hash_str = strdup(hash->valuestring);

      char *path = (char *)malloc(strlen(obj_path) + (sizeof(char) * 4) + 1);
      sprintf(path, "%s/%c%c/", obj_path, hash_str[0], hash_str[1]);

      struct stat st = {0};
      if (stat(path, &st) == -1)
      {
        mkdir_p(path, 0700);
      }

      char *file_path = (char *)malloc(strlen(path) + strlen(hash_str) + 1);
      sprintf(file_path, "%s%s", path, hash_str);

      char *url = (char *)malloc(64 + strlen(hash_str) + 1);
      sprintf(url, "https://resources.download.minecraft.net/%c%c/%s", hash_str[0], hash_str[1], hash_str);

      FILE *fp = fopen(file_path, "wb");
      curl_get(url, fp);
      fclose(fp);
    }
  }

  cJSON_Delete(json);
  free(buffer);
  fclose(fp);
  return 0;
}