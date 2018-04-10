/*
*
*   File Name       headers.h
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*
*   Title           2018-1 System programming #1-3
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
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include "hashed_path.h"

// file and directory name length
#define HASH_LENGTH             40
#define MAX_PATH_LENGTH         4096
#define EQUAL                   0

// permissions
#define MODE_777                0777
#define MODE_644                0644

// user commands
#define BYE_COMMAND             "bye"
#define CONNECT_COMMAND         "connect"
#define QUIT_COMMAND            "quit"

// directory and file names
#define CACHE_DIR_NAME          "cache"
#define LOGFILE_DIR_NAME        "logfile"
#define LOGFILE_NAME            "logfile.txt"

// logfile format
#define HIT_LOG_MESSAGE         "[Hit]"
#define MISS_LOG_MESSAGE        "[Miss]"
#define TERM_LOG_MESSAGE        "[Terminated]"
#define TERM_SERVER_MESSAGE     "**SERVER**"

// terminal text color
#define TERMINAL_COLOR_RED      "\x1b[31m"
#define TERMINAL_COLOR_GREEN    "\x1b[32m"
#define TERMINAL_COLOR_YELLOW   "\x1b[33m"
#define TERMINAL_COLOR_BLUE     "\x1b[34m"
#define TERMINAL_COLOR_MAGENTA  "\x1b[35m"
#define TERMINAL_COLOR_CYAN     "\x1b[36m"
#define TERMINAL_COLOR_RESET    "\x1b[0m"

// enum for user input type
typedef enum { bye, too_short, ok, connect, quit } input_type;

// enum for hit/miss
typedef enum { hit, miss } log_type;

// enum for bool type variables
typedef enum { false, true } bool;

// part of main function
int init(char* home_dir);
input_type check_user_input(
    char **buf,
    char *hashed_url,
    hashed_path *path,
    pid_t pid
);
void sub_process(int fd_logfile, char *hashed_url, hashed_path *path);

// utility functions
char *sha1_hash(char *input_url, char *hashed_url);
char *getHomeDir(char *home);
void remove_newline(char *string, size_t *size);
size_t get_input(char **buf, size_t *len, pid_t pid);
hashed_path *get_hash_path(char *hashed_url, hashed_path *path);
bool is_hit(hashed_path *path);
void log_user_input(int fd, log_type type, hashed_path* path);
void create_dir(char *dir_name);

#endif /* __HEADERS_H__ */
