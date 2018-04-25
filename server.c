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

#define CLIENT_INPUT_SIZE   2048

static void handler()
{
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
}

int main(void)
{
    char home_dir[MAX_PATH_LENGTH];
    int user_input;

    // file descriptor of logfile.txt
    int fd_logfile = init(home_dir);
    pid_t pid;

    struct sockaddr_in server_addr = { 0 }, client_addr = { 0 };
    int socket_fd, client_fd;
    int len;
    int reuse_addr = 1;

    if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("SERVER: cannot open stream.\n");
        return 0;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORTNO);

    // reuse port immediately
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuse_addr, sizeof(reuse_addr));
    if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("SERVER: %s\n", strerror(errno));
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
            sub_process(fd_logfile, client_fd, home_dir, &client_addr);
        }
        close(client_fd);
    }
    close(client_fd);
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
void sub_process(int fd_logfile, int client_fd, char *home_dir, struct sockaddr_in *client_addr)
{
    int hit_count = 0, miss_count = 0;
    int user_input;
    char client_input[CLIENT_INPUT_SIZE] = { 0, };
    struct tm *local_time = NULL;
    time_t start_time;
    time(&start_time);
    pid_t pid = getpid();
    char ipAddress[16] = { 0, };

    int len_out;
    hashed_path path = { 0 };
    char hashed_url[HASH_PATH_LENGTH + 1];

    printf("[%s : %d] Client was connected.\n",
        inet_ntop(AF_INET, &(client_addr->sin_addr), ipAddress, 16), ntohs(client_addr->sin_port));

    while((len_out = read(client_fd, client_input, CLIENT_INPUT_SIZE))) {
        // hash url
        sha1_hash(client_input, hashed_url);

        // get directory name and file name from checksum
        get_hash_path(hashed_url, &path);
        strcpy(path.url, client_input);

        if(strcmp(client_input, "bye") == 0) {
            break;
        }

        // if hit, print log
        if(is_hit(&path)) {
            hit_count++;
            log_user_input(fd_logfile, hit, &path, pid);
            write(client_fd, "HIT\0", 4);
        }

        // if miss, print log and make file with hashed url
        else { 
            miss_count++;
            log_user_input(fd_logfile, miss, &path, pid);
            chdir(CACHE_DIR_NAME);

            // make directory if not exists
            create_dir(path.dir_name);

            // make file if not exists
            open(path.full_path, O_CREAT, MODE_644);
            chdir("..");
            write(client_fd, "MISS\0", 5);
        }
    }
    close(client_fd);

    printf("[%s : %d] Client was disconnected.\n",
        inet_ntop(AF_INET, &(client_addr->sin_addr), ipAddress, 16), ntohs(client_addr->sin_port));

    // print log when terminating process
    dprintf(fd_logfile, "%s %s : %d | run time: %d sec. #request hit: %d, miss: %d\n",
        TERM_LOG, SERVER_PID_LOG, pid, (int)(time(NULL) - start_time), hit_count, miss_count);
    exit(0);
}