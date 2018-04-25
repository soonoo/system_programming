/*
*
*   File Name       client.c
*   Date            2018/04/26
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*   
*   Title           #2-1 Client program
*   Description     Create a socket connected to (SERVER_ADDR : PORTNO).
*                   Get url input from user and transmit into socket.
*
*/

#include "headers.h"

int main(void)
{
    int socket_fd, len;
    struct sockaddr_in server_addr = { 0 }, client_addr;
    socklen_t len_sock = sizeof(client_addr);
    char buf[INPUT_SIZE] = { 0, };

    // create a socket
    if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error: ");
        return -1;
    }

    // configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(PORTNO);

    // initiate a connection
    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() error");
        return -1;
    }

    // print client-side socket number
    printf("** Connected to " SERVER_ADDR "\n");
    if (getsockname(socket_fd, (struct sockaddr *)&client_addr, &len_sock) == -1) {
        perror("getsockname() error");
        return -1;
    }
    
    printf("** Client port number: %d\n", ntohs(client_addr.sin_port));

    while(1) {
        // get input url from user
        int input_type;
        get_input(buf, sizeof(buf));
        remove_newline(buf, (size_t)strlen(buf));

        // check user input
        input_type = check_user_input(buf);
        if(input_type == too_short) continue;
        if(input_type == bye) break;

        // transmit user input into socket
        if(write(socket_fd, buf, strlen(buf) + 1) < 0) {
            perror("write() error");
            continue;
        }

        // get response from server
        if((len = read(socket_fd, buf, sizeof(buf))) < 0) {
            perror("read() error");
            continue;
        }

        // print server response
        printf("%s\n", buf);
    }
    close(socket_fd);
    return 0;
}
