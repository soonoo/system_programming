/*
*
*   File Name       server.c
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*   
*   Title           #2-3 Server program
*   Description     Wait for connection from client.
*                   Call fork() when connection is established.
*                   Connection is handled in sub_process()
*
*/

#include "headers.h"

int fd_logfile = -1;
int sub_process_num = 0;
sem_t *sem_id;
time_t current_time;

int main(void)
{
    char home_dir[MAX_PATH_LENGTH];

    // get home directory and file descriptor of log file
    fd_logfile = init(home_dir);
    pid_t pid;

    current_time = time(NULL);
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

    // handle SIGCHILD/SIGINT signal
    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, sigint_handler);

    while(1) {
        // accept a connection on socket
        len = sizeof(client_addr);
        client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len);

        if(client_fd < 0) {
            perror("accept() error");
            return -1;
        }

        sub_process_num++;
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
*   sigint_handler
*                       
*   Description         write log when CTRL + C entered
*
*/
static void sigint_handler()
{
    if(fd_logfile != -1) {
        dprintf(fd_logfile, "%s %s run time: %d sec. #sub process: %d\n",
            TERM_SERVER_LOG, TERM_LOG, (int)(time(NULL) - current_time), sub_process_num);
    }
    sem_unlink(LOGFILE_SEMAPHORE);
    exit(0);
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

    // create semaphore
    if((sem_id = sem_open(LOGFILE_SEMAPHORE, O_CREAT, 0777, 1)) == NULL) {
        perror("sem_open() error");
        exit(0);
    }

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
    int cache_fd, len;
    char client_input[INPUT_SIZE] = { 0, };
    time(NULL);
    char *url;
    char buf[2048] = { 0, };

    hashed_path path = { 0 };
    char hashed_url[HASH_PATH_LENGTH + 1];
    log_type type;

    read(client_fd, client_input, INPUT_SIZE);
    // get URL from first line of request message
    url = get_url(client_input);

    // hash url
    sha1_hash(url, hashed_url);
    // get directory name and file name from checksum
    get_hash_path(hashed_url, &path);
    strcpy(path.url, url);

    // if hit go to skip HTTP request to origin server
    if(type = is_hit(&path)) {
        chdir(CACHE_DIR_NAME);
        cache_fd = open(path.full_path, O_RDONLY);
    }

    // if miss, make a request to origin server
    else {
        chdir(CACHE_DIR_NAME);
        // make directory if not exists
        create_dir(path.dir_name);

        // make file if not exists
        cache_fd = open(path.full_path, O_CREAT | O_RDWR, MODE_644);

        // request to origin server
        request(client_input, cache_fd);
        chdir("..");
    }

    // write cache file to browser
    while((len = read(cache_fd, buf, sizeof(buf))) > 0) {
        write(client_fd, buf, len);
        memset(buf, '\0', sizeof(char) * sizeof(buf));
    }

    close(client_fd);

    // only one process can write a log at a time
    log_user_input(fd_logfile, type, &path, sem_id);
    exit(0);
}

/*
*
*   sigalrm_handler
*   Input               int                signal
*                       
*   Description         triggered when SIGALRM occured
*
*/
void sigalrm_handler(int sig)
{
    printf("응답 없음\n");
    exit(0);
}

/*
*
*   request
*   Input               char *      request message from browser
*                       int         file descriptor of cache file
*                       
*   Description         send http request to origin server and write cache file
*
*/
void request(char *request_message, int cache_fd)
{
    struct hostent *hent = (struct hostent*)gethostbyname(get_host(request_message));
    int socket_fd, len;
    struct sockaddr_in server_addr = { 0 }, client_addr;
    char buf[2048] = { 0, };

    // create a socket
    if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error: ");
        return;
    }

    // configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*)hent->h_addr_list[0])));
    if(server_addr.sin_addr.s_addr == INADDR_NONE) {
        perror("inet_addr() error");
        return;
    }
    server_addr.sin_port = htons(HTTP_PORTNO);

    // initiate a connection
    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() error");
        return;
    }

    // write browser's message to origin server
    if(write(socket_fd, request_message, strlen(request_message) + 1) < 0) {
        perror("write() error");
        return;
    }

    // register SIGALRM handler
    signal(SIGALRM, sigalrm_handler);
    alarm(10);

    // read from origin server and write cache file
    while((len = read(socket_fd, buf, sizeof(buf))) > 0) {
        write(cache_fd, buf, len);
        memset(buf, '\0', sizeof(char) * sizeof(buf));
    }
    alarm(0);
    close(socket_fd);
}
