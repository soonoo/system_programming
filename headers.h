/*
*
*   File Name       headers.h
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*
*   Title           2018-1 System programming #1-1
*   Description     Includes standard library headers and custom functions/macros declaration
*
*/

#ifndef __HEADERS_H__
#define __HEADERS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <fcntl.h>
#include <time.h>

#include "hashed_path.h"

#define HASH_LENGTH         40
#define MAX_PATH_LENGTH     4096
#define MODE_777            0777
#define MODE_644           0644
#define BYE_COMMAND         "bye"
#define CACHE_DIR_NAME      "cache"
#define LOGFILE_DIR_NAME    "logfile"

char *sha1_hash(char *input_url, char *hashed_url);
char *getHomeDir(char *home);
void remove_newline(char *string, size_t *size);
size_t get_input(char **buf, size_t *len);
hashed_path *get_hash_path(char *hashed_url, hashed_path *path);

#endif /* __HEADERS_H__ */
