#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <sys/types.h>

int curl_get(const char *url, FILE *fp);
int mkdir_p(const char *dir, const mode_t mode);
char **str_split(char *a_str, const char a_delim, size_t *len);

#endif