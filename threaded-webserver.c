/*
 * threaded-webserver.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

int
main(int argc, char *argv[]) {

    char *dest_hostname, *dest_port;
    struct addrinfo hints, *res;
    int conn_fd;
    char buf[BUF_SIZE];
    int n;
    int rc;

    dest_hostname = argv[1];
    dest_port     = argv[2];

    /* create a socket */
    conn_fd = socket(PF_INET, SOCK_STREAM, 0);

    /* client usually doesn't bind, which lets kernel pick a port number */

    /* but we do need to find the IP address of the server */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if((rc = getaddrinfo(dest_hostname, dest_port, &hints, &res)) != 0) {
        printf("getaddrinfo failed: %s\n", gai_strerror(rc));
        exit(1);
    }

    /* connect to the server */
    if(connect(conn_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        exit(2);
    }

     printf("Connected\n");

    /* TODO: Accept new connections here */

    /* TODO: Spin new thread for each connection */
}

void 
request_handler() {

    /* TODO: Accept data from client */

    /* TODO: Parse data to figure out which HTML file one wants. Throw error is not found */

    /* TODO: Retreive specific HTML FILE and read it into buffer */

    /* TODO: Format buffer into proper format */

    /* TODO: Send HTML data back to user */

}
