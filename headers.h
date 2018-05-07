/*
*
*   File Name       headers.h
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*
*   Title           2018-1 System programming #2-2
*   Description     Includes standard libraries and custom functions/macros declaration
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "hashed_path.h"

// server configurations
#define PORTNO                  38042
#define SERVER_ADDR             "127.0.0.1"
#define HTTP_PORTNO             80

// file and directory name length
#define HASH_LENGTH             40
#define MAX_PATH_LENGTH         4096
#define EQUAL                   0
#define INPUT_SIZE              2048

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
#define HIT_LOG                 "[Hit]"
#define MISS_LOG                "[Miss]"
#define TERM_LOG                "[Terminated]"
#define TERM_SERVER_LOG         "**SERVER**"
#define SERVER_PID_LOG          "ServerPID"

// terminal text color
#define TERMINAL_COLOR_RED      "\x1b[31m"
#define TERMINAL_COLOR_GREEN    "\x1b[32m"
#define TERMINAL_COLOR_YELLOW   "\x1b[33m"
#define TERMINAL_COLOR_BLUE     "\x1b[34m"
#define TERMINAL_COLOR_MAGENTA  "\x1b[35m"
#define TERMINAL_COLOR_CYAN     "\x1b[36m"
#define TERMINAL_COLOR_RESET    "\x1b[0m"

// enum for user input type
typedef enum { bye, too_short, ok } input_type;

// enum for hit/miss
typedef enum { hit, miss } log_type;

// enum for bool type variables
typedef enum { false, true } bool;

// part of main function
int init(char* home_dir);
void sub_process(int fd_logfile, int client_fd, char *home_dir, struct sockaddr_in *client_addr);
static void sigchld_handler();

// utility functions
char *sha1_hash(char *input_url, char *hashed_url);
char *getHomeDir(char *home);
void remove_newline(char *string, size_t size);
char *get_input(char *buf, int size);
hashed_path *get_hash_path(char *hashed_url, hashed_path *path);
bool is_hit(hashed_path *path);
void log_user_input(int fd, log_type type, hashed_path* path, pid_t pid);
input_type check_user_input(char *buf);
void create_dir(char *dir_name);
char *get_url(char *buf);
char *get_host(char *buf);
void request(char *request_message);

#endif /* __HEADERS_H__ */
