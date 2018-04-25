/*
*
*   File Name       util.c
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*   
*   Title           2018-1 System programming #1-3
*   Description     Utility functions for string manipulation, getting user input
*
*/

#include "headers.h"


/*
*
*   sha1_hash
*   Input           char *      input url string to be hashed
*                   char *      hashed url string 
*   
*   Output          char *      hashed url string
*
*   Description     Hashes and returns url string to sha1 checksum
*
*/
char *sha1_hash(char *input_url, char *hashed_url)
{
    unsigned char hashed_160bits[SHA_DIGEST_LENGTH];
    char hashed_hex[HASH_LENGTH + 1];

    SHA1((unsigned char *)input_url, strlen(input_url), hashed_160bits);

    // cast to hex string
    for (int i = 0; i < sizeof(hashed_160bits); i++) {
        sprintf(hashed_hex + i*2, "%02x", hashed_160bits[i]);
    }

    strcpy(hashed_url, hashed_hex);

    return hashed_url;
}


/*
*
*   getHomeDir
*   Input           char *      pointer to home directory
*   
*   Output          char *      pointer to home directory to be returned
*
*   Description     Get and returns home directory
*
*/
char *getHomeDir(char *home)
{
    struct passwd *usr_info = getpwuid(getuid());
    strcpy(home, usr_info->pw_dir);

    return home;
}


/*
*
*   remove_newline
*   Input           char *      string with newline at the end
*                   size_t *    size of string
*
*   Description     removes new line character at the end of string
*
*/
void remove_newline(char *string, size_t size)
{
    if (size == 0 || string == NULL) return;
    if (string[size - 1] == '\n') {
        string[size - 1] = '\0';
    }
}


/*
*
*   get_input
*   Input           char **     double pointer to a user input
*                   size_t *    buffer
*                   pid_t       0 if child process, or parent process
*
*   Output          size_t      size of user input
*
*   Description     get user input using getline()
*
*/
char *get_input(char *buf, int size)
{
    printf("input URL > ");

    return fgets(buf, size, stdin);
}


/*
*
*   get_hash_path
*   Input           char *          hashed url string
*                   hashed_path     pointer to path string to be calculated
*
*   Output          hashed_path *   pointer to a file path
*
*   Description     parse hashed url to path string
*
*/
hashed_path *get_hash_path(char *hashed_url, hashed_path *path)
{
    if (hashed_url == NULL || path == NULL) return NULL;

    // get directory name and file name from 160 bits checksum
    strncpy(path->dir_name, hashed_url, DIR_NAME_LENGTH);
    strncpy(path->file_name, hashed_url + DIR_NAME_LENGTH, FILE_NAME_LENGTH);

    // save full path to path->full_path e.g. "4AF/A71FFB32DC64586C1A3B65CB7B99D7F2E3185"
    strncpy(path->full_path, path->dir_name, DIR_NAME_LENGTH);
    strncpy(path->full_path + DIR_NAME_LENGTH, "/", 1);
    strncpy(path->full_path + DIR_NAME_LENGTH + 1, path->file_name, FILE_NAME_LENGTH);

    return path;
}


/*
*
*   is_hit
*   Input           hashed_path*    hashed url string
*
*   Output          bool            true if url is hit
*
*   Description     return true if url is hit
*
*/
bool is_hit(hashed_path *path)
{
    DIR *dp_cache = opendir(CACHE_DIR_NAME);
    struct dirent *dir_cache;

    // iterate cache directory
    while((dir_cache = readdir(dp_cache))) {
        if (strcmp(dir_cache->d_name, path->dir_name) == EQUAL) {
            chdir(CACHE_DIR_NAME);
            DIR *dp_hash = opendir(path->dir_name);
            struct dirent *dir_hash;

            // iterate directory in cache directory
            while((dir_hash = readdir(dp_hash))) {
                if(strcmp(dir_hash->d_name, path->file_name) == EQUAL) {
                    closedir(dp_hash);
                    chdir("..");

                    // return true when hit
                    return true;
                }
            }
            closedir(dp_hash);
            chdir("..");
        }
    }
    closedir(dp_cache);

    // return false when miss
    return false;
}

/*
*
*   log_user_input
*   Input           int                 file descriptor of logfile.txt
*                   enum log_type       one of three log message type: hit, miss, terminate
*                   hasned_path *       hashed path string
*
*   Output          void
*
*   Description     print log message to file descriptor using dprintf() function
*
*/
void log_user_input(int fd, log_type type, hashed_path* path, pid_t pid)
{
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);
    switch(type) {
        case hit:
            dprintf(fd, "%s %s : %d | %s/%s-[%d/%02d/%02d, %02d:%02d:%02d]\n",
                HIT_LOG, SERVER_PID_LOG, pid, path->dir_name, path->file_name,
                1900 + local_time->tm_year, local_time->tm_mon + 1, local_time->tm_mday,
                local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
            dprintf(fd, "%s %s\n", HIT_LOG, path->url);
            return;
        case miss:
            dprintf(fd, "%s %s : %d | %s-[%d/%02d/%02d, %02d:%02d:%02d]\n",
                MISS_LOG, SERVER_PID_LOG, pid, path->url,
                1900 + local_time->tm_year, local_time->tm_mon + 1, local_time->tm_mday,
                local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
            return;
    }
}

/*
*
*   create_dir
*   Input           char*        directory name to create
*
*   Output          void
*
*   Description     create directory if not exists
*
*/
void create_dir(char *dir_name)
{
    DIR *dp;
    if((dp = opendir(dir_name)) == NULL) mkdir(dir_name, MODE_777);
    if(dp) closedir(dp);
}

/*
*
*   check_user_input
*   Input               char **             pointer to user input
*                       pid_t               0 if child process, or parent process
*
*   Output              enum input_type     0 when command equals "bye"
*                                           1 when command is too short
*                                           2 when command is valid
*
*   Description         get user input and check if bye command is entered
*
*/
input_type check_user_input(char *buf)
{
    // quit if input is too short
    if (strlen(buf) < 1) return too_short;

    // return command type
    if(strcmp(BYE_COMMAND, buf) == EQUAL) return bye;
    if(strcmp(CONNECT_COMMAND, buf) == EQUAL) return _connect;
    if(strcmp(QUIT_COMMAND, buf) == EQUAL) return quit;
    return ok;
}