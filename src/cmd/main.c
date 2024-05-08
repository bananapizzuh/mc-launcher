#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "manifest.h"
#include "version.h"
#include <sys/utsname.h>

int main()
{
  latest_version latest;
  version_info *version_info_array;
  struct utsname uts;
  uname(&uts);

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
  printf("System is %s\n", uts.sysname);

  char *v1_20_6 = version_info_array[1].url;

  get_version_json("assets/version.json", v1_20_6);
  parse_version_json("assets/version.json", "assets/libraries/");
  return 0;
}