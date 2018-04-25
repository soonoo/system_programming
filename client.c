#include "headers.h"

#define INPUT_SIZE  1024

int main(void)
{
    int socket_fd, len;
    struct sockaddr_in server_addr = { 0 };
    char proxy_addr[] = "127.0.0.1";
    char buf[INPUT_SIZE] = { 0, };

    if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("cannot open stream\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(proxy_addr);
    server_addr.sin_port = htons(PORTNO);

    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("cannot connect\n");
        return -1;
    }

    struct sockaddr_in sin;
    socklen_t len_sock = sizeof(sin);
    if (getsockname(socket_fd, (struct sockaddr *)&sin, &len_sock) == -1)
        perror("getsockname");
    else
        printf("port number %d\n", ntohs(sin.sin_port));

    while(1) {
        int input_type;
        get_input(buf, sizeof(buf));
        remove_newline(buf, (size_t)strlen(buf));

        input_type = check_user_input(buf);
        if(input_type == too_short) continue;
        if(input_type == bye) break;

        if(write(socket_fd, buf, strlen(buf) + 1) > 0) {
            if((len = read(socket_fd, buf, sizeof(buf))) > 0) {
                printf("%s\n", buf);
            }
        }
    }
    close(socket_fd);
    return 0;
}
