/*
*
*   File Name       server.c
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*   
*   Title           #2-2 Server program
*   Description     Wait for connection from client.
*                   Call fork() when connection is established.
*                   Connection is handled in sub_process()
*
*/

#include "headers.h"

int main(void)
{
    char home_dir[MAX_PATH_LENGTH];

    // get home directory and file descriptor of log file
    int fd_logfile = init(home_dir);
    pid_t pid;

    struct sockaddr_in server_addr = { 0 }, client_addr = { 0 };
    int socket_fd, client_fd;
    int len;
    int reuse_addr = 1;

    // create a socket
    if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error");
        return -1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORTNO);

    // reuse port immediately
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuse_addr, sizeof(reuse_addr));
    if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind() error");
        return -1;
    }

    // mark the socket_fd as a passive socket
    listen(socket_fd, 10);
    printf("listening on port %d\n", PORTNO);

    // handle SIGCHILD signal
    signal(SIGCHLD, sigchld_handler);

    while(1) {
        // accept a connection on socket
        len = sizeof(client_addr);
        client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len);

        if(client_fd < 0) {
            perror("accept() error");
            return -1;
        }

        pid = fork();
        
        // fork() error
        if(pid == -1) {
            perror("fork() error");
            close(client_fd);
            close(socket_fd);
            close(fd_logfile);
            return -1;
        }

        // child process
        if(pid == 0) {
            sub_process(fd_logfile, client_fd, home_dir, &client_addr);
        }

        // parent process closes socket and continue to get uer input
        close(client_fd);
    }
    close(socket_fd);
    close(fd_logfile);
    return 0;
}

/*
*
*   sigchld_handler
*
*   Description     handler for SIGCHLD signal from child process
*
*/
static void sigchld_handler()
{
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
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
*   Input               int                         file descriptor of logfile.txt
*                       int                         file descriptor of socket
*                       char *                      pointer to home directory string
*                       struct sockaddr_in *        sockaddr_in struct of client address
*                       
*   Description         Get user input from socket and transmit hit/miss to client.
*                       Make file if miss.
*                       Log to logfile.txt at each user input.
*                       Exit when user input equals "bye"
*
*/
void sub_process(int fd_logfile, int client_fd, char *home_dir, struct sockaddr_in *client_addr)
{
    int hit_count = 0, miss_count = 0;
    int cache_fd, len;
    char client_input[INPUT_SIZE] = { 0, };
    time_t start_time;
    time(&start_time);
    pid_t pid = getpid();
    char response[8192] = { 0, };
    char *index = response;
    char *url;
    char buf[4096] = { 0, };

    hashed_path path = { 0 };
    char hashed_url[HASH_PATH_LENGTH + 1];

    read(client_fd, client_input, INPUT_SIZE);
    // get URL from first line of request message
    url = get_url(client_input);

    // hash url
    sha1_hash(url, hashed_url);
    // get directory name and file name from checksum
    get_hash_path(hashed_url, &path);
    strcpy(path.url, url);

    printf("%s ", path.url);
    // if hit, print log
    if(is_hit(&path)) {
        printf("HIT\n");
        hit_count++;
        log_user_input(fd_logfile, hit, &path, pid);
        chdir(CACHE_DIR_NAME);

        cache_fd = open(path.full_path, O_RDONLY);

        while((len = read(cache_fd, buf, sizeof(buf))) > 0) {
            write(client_fd, buf, len);
            memset(buf, '\0', sizeof(char) * sizeof(buf));
        }
    }

    // if miss, print log and make file with hashed url
    else {
        printf("MISS");
        miss_count++;
        log_user_input(fd_logfile, miss, &path, pid);

        chdir(CACHE_DIR_NAME);
        // make directory if not exists
        create_dir(path.dir_name);

        // make file if not exists
        printf(" %s", path.full_path);
        cache_fd = open(path.full_path, O_CREAT | O_WRONLY, MODE_644);

        request(client_input, client_fd, cache_fd);
        printf(" %d\n", cache_fd);
        chdir("..");
    }

    // print log when terminating process
    dprintf(fd_logfile, "%s %s : %d | run time: %d sec. #request hit: %d, miss: %d\n",
        TERM_LOG, SERVER_PID_LOG, pid, (int)(time(NULL) - start_time), hit_count, miss_count);

    exit(0);
}


void handle_alarm(int sig)
{
    printf("응답 없음\n");
    exit(0);
}

void request(char *request_message, int client_fd, int cache_fd)
{
    struct hostent *hent = (struct hostent*)gethostbyname(get_host(request_message));
    int socket_fd, len;
    struct sockaddr_in server_addr = { 0 }, client_addr;
    socklen_t len_sock = sizeof(client_addr);
    char buf[8192] = { 0, };

    // create a socket
    if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error: ");
        return;
    }

    // configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*)hent->h_addr_list[0])));
    server_addr.sin_port = htons(HTTP_PORTNO);

    // initiate a connection
    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() error");
        return;
    }

    if(write(socket_fd, request_message, strlen(request_message) + 1) < 0) {
        perror("write() error");
        return;
    }
    signal(SIGALRM, handle_alarm);

    alarm(10);
    while((len = read(socket_fd, buf, sizeof(buf))) > 0) {
        write(client_fd, buf, len);
        write(cache_fd, buf, len);
        memset(buf, '\0', sizeof(char) * sizeof(buf));
    }
    close(client_fd);    
    close(socket_fd);
}
