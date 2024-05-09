#ifndef VERSION_H
#define VERSION_H

int get_version_json(const char *file_path, const char *url);
int parse_version_json(const char *file_path, const char *lib_path, const char *asset_idx_path);

#endif