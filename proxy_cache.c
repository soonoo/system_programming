/*
*
*   File Name       main.c
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*   
*   Title           2018-1 System programming #1-1
*   Description     Gets an input URL from stdin.
*                   If the URL was first entered, create the directory with hash value
*
*/

#include "headers.h"

int main(void)
{
    char *buf = NULL;
    size_t size, len;

    char hashed_url[HASH_PATH_LENGTH + 1];
    char home_dir[MAX_PATH_LENGTH];
    hashed_path path = { 0 };
    struct tm *local_time = NULL;
    DIR *dp;
    struct dirent *dir;
    int fd_logfile;
    time_t current_time;
    time_t start_time;
    time(&start_time);

    // set file mode mask
    umask(0000);

    // get home directory and change working directory to home directory
    getHomeDir(home_dir);
    chdir(home_dir);

    // create "logfile" directory
    if((dp = opendir(LOGFILE_DIR_NAME)) == NULL) mkdir(LOGFILE_DIR_NAME, MODE_755);
    if(dp) closedir(dp);
    chdir(LOGFILE_DIR_NAME);

    fd_logfile = open(LOGFILE_NAME, O_RDWR | O_CREAT | O_APPEND, MODE_644);
    chdir("..");

    // create "cache" directory and change working directory
    if((dp = opendir(CACHE_DIR_NAME)) == NULL) mkdir(CACHE_DIR_NAME, MODE_777);
    if(dp) closedir(dp);

    while(1) {
        // get user input
        size = get_input(&buf, &len);
        time(&current_time);

        local_time = localtime(&current_time);
        remove_newline(buf, &size);
        if (size < 1) continue;

        // quit if BYE COMMAND is entered
        if(strcmp(BYE_COMMAND, buf) == 0) break;
        // hash url
        sha1_hash(buf, hashed_url);

        // get directory namd and file name
        get_hash_path(hashed_url, &path);
        strcpy(path.url, buf);

        if(is_hit(&path)) {
            dprintf(fd_logfile, "%s %s/%s-[%d/%02d/%02d, %02d:%02d:%02d]\n",
                HIT_LOG_MESSAGE, path.dir_name, path.file_name,
                1900 + local_time->tm_year, local_time->tm_mon, local_time->tm_mday,
                local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
            dprintf(fd_logfile, "%s %s\n", HIT_LOG_MESSAGE, path.url);
        } else {
            dprintf(fd_logfile, "%s %s\n", MISS_LOG_MESSAGE, path.url);
            chdir(CACHE_DIR_NAME);

            // make directory if not exists
            if((dp = opendir(path.dir_name)) == NULL) mkdir(path.dir_name, MODE_777);
            if(dp) closedir(dp);

            // make file if not exists
            open(path.full_path, O_CREAT, MODE_644);

            // write logfile
            chdir("..");
        }
    }

    free(buf);
    return 0;
}


/*
*
*   sha_hash
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
