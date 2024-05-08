#include "util.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

typedef struct
{
  char *release;
  char *snapshot;
} latest_version;

typedef struct
{
  char *id;
  char *type;
  char *url;
  char *time;
  char *release_time;
  char *sha1;
  int compliance_level;
} version_info;

int get_manifest(const char *file_path)
{
  FILE *fp = fopen(file_path, "wb");
  if (fp == NULL)
  {
    printf("Error opening manifest.json file.\n");
    return 1;
  }

  int status = curl_get("https://piston-meta.mojang.com/mc/game/version_manifest_v2.json", fp);
  fclose(fp);
  return status;
}

int parse_manifest(const char *file_path, latest_version *latest_version, version_info **version_info_array)
{
  FILE *fp;
  char *buffer;
  long file_size;
  size_t result;
  cJSON *json;
  cJSON *latest;

  // fp is file pointer
  fp = fopen(file_path, "rb");

  if (fp == NULL)
  {
    printf("Error opening manifest.json file.\n");
    return 1;
  }

  fseek(fp, 0, SEEK_END); // positions file pointer at end of file
  file_size = ftell(fp);  // ftell determins the current position of the file pointer, assines value to file size,
  rewind(fp);             // puts file pointer at begining of file

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

  // parse
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

  latest = cJSON_GetObjectItemCaseSensitive(json, "latest");
  if (cJSON_IsObject(latest))
  {
    latest_version->release = cJSON_GetObjectItemCaseSensitive(latest, "release")->valuestring;
    latest_version->snapshot = cJSON_GetObjectItemCaseSensitive(latest, "snapshot")->valuestring;
  }

  cJSON *versions = cJSON_GetObjectItemCaseSensitive(json, "versions");
  if (cJSON_IsArray(versions))
  {
    int array_size = cJSON_GetArraySize(versions);
    *version_info_array = (version_info *)malloc(sizeof(version_info) * array_size);
    if (version_info_array == NULL)
    {
      fprintf(stderr, "Memory error.");
      exit(1);
    }

    if (version_info_array == NULL)
    {
      fprintf(stderr, "Memory error.");
      exit(1);
    }

    for (int i = 0; i < array_size; i++)
    {
      cJSON *version = cJSON_GetArrayItem(versions, i);
      version_info version_info;
      version_info.id = cJSON_GetObjectItemCaseSensitive(version, "id")->valuestring;
      version_info.type = cJSON_GetObjectItemCaseSensitive(version, "type")->valuestring;
      version_info.url = cJSON_GetObjectItemCaseSensitive(version, "url")->valuestring;
      version_info.time = cJSON_GetObjectItemCaseSensitive(version, "time")->valuestring;
      version_info.release_time = cJSON_GetObjectItemCaseSensitive(version, "releaseTime")->valuestring;
      version_info.sha1 = cJSON_GetObjectItemCaseSensitive(version, "sha1")->valuestring;
      version_info.compliance_level = cJSON_GetObjectItemCaseSensitive(version, "complianceLevel")->valueint;

      (*version_info_array)[i] = version_info;
    }
  }

  free(buffer);
  return 0;
}