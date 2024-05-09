#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "manifest.h"
#include "version.h"
#include "asset_index.h"
#include <sys/utsname.h>

int main()
{
  latest_version latest;
  version_info *version_info_array;
  struct utsname uts;
  uname(&uts);

  struct stat st = {0};
  if (stat("mc", &st) == -1)
  {
    mkdir("mc", 0700);
  }

  if (get_manifest("mc/manifest.json") != 0)
  {
    fprintf(stderr, "Error getting manifest.json file.\n");
    return 1;
  }
  if (parse_manifest("mc/manifest.json", &latest, &version_info_array) != 0)
  {
    fprintf(stderr, "Error parsing manifest.json file.\n");
  }
  printf("System is %s\n", uts.sysname);

  char *v1_20_6 = version_info_array[1].url;

  get_version_json("mc/versions/1.20.6.json", v1_20_6);
  parse_version_json("mc/version.json", "mc/libraries/", "mc/assets/indexes/");
  // parse_asset_index("mc/assets/indexes/16.json", "mc/assets/objects");
  return 0;
}