#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include "util.h"

#if defined(_WIN32) || defined(_WIN64)
const char *os = "windows";
#else
#ifdef __linux
const char *os = "linux";
#else
const char *os = "mac";
#endif
#endif

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

int parse_version_json(const char *file_path, const char *lib_path, const char *asset_idx_path, const char *client_jar_path)
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

  cJSON *downloads = cJSON_GetObjectItemCaseSensitive(json, "downloads");
  if (cJSON_IsObject(downloads))
  {
    cJSON *client = cJSON_GetObjectItemCaseSensitive(downloads, "client");
    if (cJSON_IsObject(client))
    {
      cJSON *url = cJSON_GetObjectItemCaseSensitive(client, "url");
      if (cJSON_IsString(url) && (url->valuestring != NULL))
      {
        struct stat st = {0};
        char *token_path = strdup(client_jar_path);
        token_path[strlen(token_path) - strlen(strrchr(token_path, '/')) + 1] = '\0';
        printf("token_path%s\n", token_path);
        if (stat(token_path, &st) == -1)
        {
          mkdir_p(token_path, 0700);
        }
        FILE *client_file;
        printf("path: %s\n", client_jar_path);
        client_file = fopen(client_jar_path, "w");
        if (client_file == NULL)
        {
          fprintf(stderr, "Error in opning File");
          exit(1);
        }
        curl_get(cJSON_GetObjectItemCaseSensitive(client, "url")->valuestring, client_file);
        fclose(client_file);
      }
    }
  }

  cJSON *asset_index = cJSON_GetObjectItemCaseSensitive(json, "assetIndex");
  if (cJSON_IsObject(asset_index))
  {

    cJSON *url = cJSON_GetObjectItemCaseSensitive(asset_index, "url");
    if (cJSON_IsString(url) && (url->valuestring != NULL))
    {
      struct stat st = {0};
      if (stat(asset_idx_path, &st) == -1)
      {
        printf("path: %s\n", asset_idx_path);
        mkdir_p(asset_idx_path, 0700);
      }

      char *path = malloc(strlen(asset_idx_path) + strlen(basename(url->valuestring)) + 1);
      sprintf(path, "%s%s", asset_idx_path, basename(url->valuestring));
      printf("path: %s\n", path);

      FILE *asset_idx = fopen(path, "w");
      curl_get(cJSON_GetObjectItemCaseSensitive(asset_index, "url")->valuestring, asset_idx);
      fclose(asset_idx);
    }
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
            size_t len = 0;
            char **tokens = str_split(path_json->valuestring, '/', &len);
            name = tokens[len / 8 - 2];
            printf("name: %s\n", name);

            size_t path_len = 0;
            for (size_t i = 0; i < len / 8 - 2; i++)
            {
              path_len += strlen(tokens[i]);
            }

            char *token_path = malloc((sizeof(char *) * path_len) + 1);
            token_path[0] = '\0';
            for (size_t i = 0; i < len / 8 - 2; i++)
            {
              strcat(token_path, tokens[i]);
              strcat(token_path, "/");
            }

            path = malloc(strlen(lib_path) + strlen(token_path) + 1);
            sprintf(path, "%s%s", lib_path, token_path);

            struct stat st = {0};
            if (stat(path, &st) == -1)
            {
              mkdir_p(path, 0700);
            }
          }

          cJSON *url_json = cJSON_GetObjectItemCaseSensitive(artifact, "url");
          if (cJSON_IsString(url_json) && (url_json->valuestring != NULL))
          {
            url = url_json->valuestring;
          }

          char *final_path = malloc(strlen(path) + strlen(name) + 1);
          sprintf(final_path, "%s%s", path, name);
          printf("final path: %s\n", final_path);
          FILE *lib = fopen(final_path, "wb");
          curl_get(url, lib);
        }
      }
    }
  }
}