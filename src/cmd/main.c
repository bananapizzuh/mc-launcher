#include <stdio.h>
#include "manifest.h"

int main()
{
  latest_version latest;
  version_info *version_info_array;

  if (get_manifest("manifest.json") == 0)
  {

    if (parse_manifest("manifest.json", &latest, &version_info_array) == 0)
    {
      printf("Latest release: %s\n", latest.release);
      printf("Latest snapshot: %s\n", latest.snapshot);
      printf("%s", version_info_array[10].id);
    }
    else
    {
      printf("Error parsing manifest.json file.\n");
      return 1;
    }
  }
  else
  {
    printf("Error getting manifest.json file.\n");
    return 1;
  }
  return 0;
}