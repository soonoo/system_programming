/*
*
*   File Name       headers.h
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*
*   Title           2018-1 System programming #2-3
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
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>

#include "hashed_path.h"

// server configurations
#define PORTNO                  38042
#define SERVER_ADDR             "127.0.0.1"
#define HTTP_PORTNO             80

// file and directory name length
#define HASH_LENGTH             40
#define MAX_PATH_LENGTH         4096
#define EQUAL                   0
#define INPUT_SIZE              4096

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
#define LOGFILE_SEMAPHORE       "logfile_semaphore"

// logfile format
#define HIT_LOG                 "[Hit]"
#define MISS_LOG                "[Miss]"
#define TERM_LOG                "[Terminated]"
#define TERM_SERVER_LOG         "**SERVER**"
#define SERVER_PID_LOG          "ServerPID"

// enum for user input type
typedef enum { bye, too_short, ok } input_type;

// enum for hit/miss
typedef enum { miss, hit } log_type;

// enum for bool type variables
typedef enum { false, true } bool;

// external variables for thread
extern int fd_logfile;
extern sem_t* sem_id;

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
void log_user_input(int fd, log_type type, hashed_path* path, sem_t *sem_id);
input_type check_user_input(char *buf);
void create_dir(char *dir_name);
char *get_url(char *buf);
char *get_host(char *buf);
void sigalrm_handler(int sig);
static void sigint_handler();
void request(char *request_message, int cache_fd);
void *thread_print();

#endif /* __HEADERS_H__ */
