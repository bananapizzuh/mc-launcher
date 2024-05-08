#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "manifest.h"
#include "version.h"

int main()
{
  latest_version latest;
  version_info *version_info_array;

  struct stat st = {0};
  if (stat("assets", &st) == -1)
  {
    mkdir("assets", 0700);
  }

  if (get_manifest("assets/manifest.json") != 0)
  {
    fprintf(stderr, "Error getting manifest.json file.\n");
    return 1;
  }
  if (parse_manifest("assets/manifest.json", &latest, &version_info_array) != 0)
  {
    fprintf(stderr, "Error parsing manifest.json file.\n");
  }

  printf("Latest release: %s\n", latest.release);
  printf("Latest snapshot: %s\n", latest.snapshot);
  printf("%s\n", version_info_array[10].id);
  printf("%s\n", version_info_array[10].url);

  char *test_version = version_info_array[10].url;
  printf("Version 10 id: %s\n", test_version);
  free(version_info_array);

  get_version_json("assets/version.json", test_version);
  return 0;
}