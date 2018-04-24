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

    printf("> ");
    while((len = read(STDOUT_FILENO, buf, sizeof(buf))) > 0) {
        if(write(socket_fd, buf, strlen(buf)) > 0) {
            if((len = read(socket_fd, buf, sizeof(buf))) > 0) {
                printf("%s\n", buf);
                bzero(buf, sizeof(buf));
            }
        }
        printf("> ");
    }
    close(socket_fd);
    return 0;
}
