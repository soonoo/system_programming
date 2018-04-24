/*
*
*   File Name       proxy_cache.c
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*   
*   Title           2018-1 System programming #1-3
*   Description     make a child process when 'connect' input id entered
*                   quit when 'quit' command is entered
*
*/

#include "headers.h"

#define CLIENT_INPUT_SIZE   1024

static void handler()
{
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
}

int main(void)
{
    char *buf = NULL;

    char hashed_url[HASH_PATH_LENGTH + 1];
    char home_dir[MAX_PATH_LENGTH];
    hashed_path path = { 0 };
    time_t start_time;
    time(&start_time);
    int user_input;

    // file descriptor of logfile.txt
    int fd_logfile = init(home_dir);
    pid_t pid;
    int process_count = 0;

    //
    struct sockaddr_in server_addr = { 0 }, client_addr = { 0 };
    int socket_fd, client_fd;
    int len, len_out;
    char client_input[CLIENT_INPUT_SIZE] = { 0, };

    if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("SERVER: cannot open stream.\n");
        return 0;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORTNO);

    if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("SERVER: cannot bind local address.\n");
        return 0;
    }
    listen(socket_fd, 10);
    signal(SIGCHLD, (void *)handler);

    while(1) {
        len = sizeof(client_addr);
        client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len);

        if(client_fd < 0) {
            printf("SERVER: accept failed.");
            return 0;
        }

        pid = fork();
        
        if(pid == -1) {
            close(client_fd);
            close(socket_fd);
            continue;
        }
        if(pid == 0) {
            while((len_out = read(client_fd, client_input, CLIENT_INPUT_SIZE))) {
                if(strcmp(client_input, "bye") == 0) {
                    break;
                }
                printf("%s\n", client_input);
                write(client_fd, client_input, len_out);
            }
            close(client_fd);
            exit(0);
        }
        close(client_fd);
    }
    close(client_fd);
    return 0;

    // parent process starts
    while(1) {
        // get user input
        user_input = check_user_input(&buf, hashed_url, &path, pid);

        // quit if command equals "quit" or fork if equals "connect"
        if(user_input == quit) break;
        else if(user_input == _connect) {
            process_count++;
            pid = fork();
        } else continue;

        if(pid != 0) {
            // parent process
            wait(NULL);
            continue;
        } else {
            // child process
            sub_process(fd_logfile, hashed_url, &path);
        }
    }

    // print log when terminating program
    dprintf(fd_logfile, "%s %s run time: %d sec. #sub process: %d\n",
        TERM_SERVER_MESSAGE, TERM_LOG_MESSAGE, (int)(time(NULL) - start_time), process_count);
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
*                       char *              hashed url
*                       hashed_path *       pointer to struct with have dir/file name
*                       pid_t               0 if child process, or parent process
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
    char *hashed_url,
    hashed_path *path,
    pid_t pid)
{
    size_t size, len;

    // get user input
    size = get_input(buf, &len, pid);
    remove_newline(*buf, &size);

    // quit if input is too short
    if (size < 1) return too_short;

    // return command type
    if(strcmp(BYE_COMMAND, *buf) == EQUAL) return bye;
    if(strcmp(CONNECT_COMMAND, *buf) == EQUAL) return _connect;
    if(strcmp(QUIT_COMMAND, *buf) == EQUAL) return quit;

    // hash url
    sha1_hash(*buf, hashed_url);

    // get directory name and file name from checksum
    get_hash_path(hashed_url, path);
    strcpy(path->url, *buf);
    return ok;
}


/*
*
*   sub_process
*   Input               int                 file descriptor of logfile.txt
*                       char *              hashed url
*                       hashed_path *       pointer to struct which has dir/file name
*
*   Description         get user input and determine hit/miss.
*                       make file if miss.
*                       log to logfile.txt at each user input.
*                       exit when user input equals "bye"
*
*/
void sub_process(int fd_logfile, char *hashed_url, hashed_path *path)
{
    int hit_count = 0, miss_count = 0;
    int user_input;
    char *buf = NULL;
    struct tm *local_time = NULL;
    time_t start_time;
    time(&start_time);

    while(1) {        
        user_input = check_user_input(&buf, hashed_url, path, 0);
        if(user_input == bye) break;
        if(user_input == too_short) continue;

        // if hit, print log
        if(is_hit(path)) {
            hit_count++;
            log_user_input(fd_logfile, hit, path);
        }

        // if miss, print log and make file with hashed url
        else { 
            miss_count++;
            log_user_input(fd_logfile, miss, path);
            chdir(CACHE_DIR_NAME);

            // make directory if not exists
            create_dir(path->dir_name);

            // make file if not exists
            open(path->full_path, O_CREAT, MODE_644);
            chdir("..");
        }
    }
    // print log when terminating process
    dprintf(fd_logfile, "%s run time: %d sec. #request hit: %d, miss: %d\n",
        TERM_LOG_MESSAGE, (int)(time(NULL) - start_time), hit_count, miss_count);
    exit(0);
}