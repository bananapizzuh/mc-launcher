#ifndef MANIFEST_H
#define MANIFEST_H

#include <stdio.h>

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

int get_manifest(const char *file_path);
int parse_manifest(const char *file_path, latest_version *latest_version, version_info *version_info_array[]);

#endif