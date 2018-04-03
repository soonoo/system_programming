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

    char hashed_url[HASH_PATH_LENGTH + 1];
    char home_dir[MAX_PATH_LENGTH];
    hashed_path path = { 0 };
    struct tm *local_time = NULL;
    int fd_logfile;
    time_t current_time;
    time_t start_time;
    time(&start_time);
    int hit_count, miss_count = 0;
    int user_input;

    // file descriptor of logfile.txt
    fd_logfile = init(home_dir);

    while(1) {
        // quit if bye command entered, continue loop if input is too short
        user_input = check_user_input(&buf, &current_time, &local_time, hashed_url, &path);
        if(user_input == too_short) continue;
        if(user_input == bye) break;

        // if hit, print log
        if(is_hit(&path)) {
            hit_count++;
            log_user_input(fd_logfile, hit, local_time, &path);
        }

        // if miss, print log and make file with hashed url
        else { 
            miss_count++;
            log_user_input(fd_logfile, miss, local_time, &path);
            chdir(CACHE_DIR_NAME);

            // make directory if not exists
            create_dir(path.dir_name);

            // make file if not exists
            open(path.full_path, O_CREAT, MODE_644);
            chdir("..");
        }
    }

    // print log when terminating program
    dprintf(fd_logfile, "%s run time: %d sec. #request hit: %d, miss: %d\n",
        TERM_LOG_MESSAGE, (int)(time(NULL) - start_time), hit_count, miss_count);
    free(buf);
    return 0;
}


/*
*
*   init
*   Input           char *      pointer to home directory name
*   
*   Output          int         file descriptor of logfile.txt
*
*   Description     initialize things.
*                    1) set umask
*                    2) get home directory
*                    3) create cache/logfile directory
*                    4) make logfile.txt
*
*/
int init(char* home_dir)
{
    int fd;

    // set file mode mask
    umask(0000);

    // get home directory and change working directory to home directory
    getHomeDir(home_dir);
    chdir(home_dir);

    // create "logfile" directory
    create_dir(LOGFILE_DIR_NAME);
    chdir(LOGFILE_DIR_NAME);

    fd = open(LOGFILE_NAME, O_RDWR | O_CREAT | O_APPEND, MODE_777);
    chdir("..");

    // create "cache" directory and change working directory
    create_dir(CACHE_DIR_NAME);

    return fd;
}

/*
*
*   check_user_input
*   Input               char **             pointer to user input
*                       time_t *            pointer to current time
*                       tm **               double pointer to local time
*                       char *              hashed url
*                       hashed_path *       pointer to struct with have dir/file name
*
*   Output              enum input_type     0 when command equals "bye"
*                                           1 when command is too short
*                                           2 when command is valid
*
*   Description         get user input and check if bye command is entered
*                        1) set umask
*                        2) get home directory
*                        3) create cache/logfile directory
*                        4) make logfile.txt
*
*/
input_type check_user_input(
    char **buf,
    time_t *current_time,
    struct tm **local_time,
    char *hashed_url,
    hashed_path* path)
{
    size_t size, len;

    // get user input
    size = get_input(buf, &len);
    remove_newline(*buf, &size);

    // quit if input is too short
    if (size < 1) return too_short;

    // get current local time 
    time(current_time);
    *local_time = localtime(current_time);

    // quit if BYE COMMAND is entered
    if(strcmp(BYE_COMMAND, *buf) == EQUAL) return bye;

    // hash url
    sha1_hash(*buf, hashed_url);

    // get directory name and file name from checksum
    get_hash_path(hashed_url, path);
    strcpy(path->url, *buf);
    return ok;
}