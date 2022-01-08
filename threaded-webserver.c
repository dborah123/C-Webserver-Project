/*
 * threaded-webserver.c
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>

#define BUF_SIZE 2000
#define BACKLOG 10

void *service_connection(void *data);

int
main(int argc, char *argv[]) {

    char *listen_port;
    int listen_fd, conn_fd;
    struct addrinfo hints, *res;
    int rc;
    struct sockaddr_in remote_sa;
    uint16_t remote_port;
    socklen_t addrlen;
    char *remote_ip;
    char buf[BUF_SIZE];
    int bytes_received;
    int *conn_fd_heap;

    // Placeholder
    pthread_t thread;

    listen_port = argv[1];

    /* create a socket */
    if ((listen_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
    }

    /* bind it to a port */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if((rc = getaddrinfo(NULL, listen_port, &hints, &res)) != 0) {
        printf("getaddrinfo failed: %s\n", gai_strerror(rc));
        exit(1);
    }

    if (bind(listen_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind");
    }

    /* start listening */
    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen");
    }


    printf("Threaded HTTP Server waiting on port %s\n", listen_port);

    addrlen = sizeof(remote_sa);

    /* TODO: Accept new connections here */
    while (1) {

        conn_fd = accept(listen_fd, (struct sockaddr *) &remote_sa, &addrlen);

        if (conn_fd < 0) {
            perror("accept");
            continue;
        }

        /* Mallocing conn_fd for child function thread to avoid any threading errors */
        conn_fd_heap = malloc(sizeof(int));

        if (conn_fd_heap == NULL) {
            perror("malloc");
        }

        *conn_fd_heap = conn_fd;

        /* Announcing new communication partner */
        remote_ip = inet_ntoa(remote_sa.sin_addr);
        remote_port = ntohs(remote_sa.sin_port);

        printf("Connection from %s: %d\n", remote_ip, remote_port);

        /* Spinning a new thread to service connection */
        if (pthread_create(&thread, NULL, service_connection, conn_fd_heap) != 0) {
            perror("pthread_create");
        }
    }
}

void * 
service_connection(void * data) {
    /**
     * Services connections for server, providing HTML pages.
     * Intended to use in a thread
     */

    int conn_fd = *(int *) data;
    free(data);
    int bytes_received;
    int bytes_read;
    int home_html_fd;
    char input_buf[BUF_SIZE];
    char html_buf[BUF_SIZE];
    char output_buf[BUF_SIZE];

    /* TODO: Accept data from client */
    while ((bytes_received = recv(conn_fd, input_buf, BUF_SIZE, 0)) > 0) {

        /* TODO: Parse data to figure out which HTML file one wants. Throw error is not found */

        /* TODO: Retreive specific HTML FILE and read it into buffer */
        if ((home_html_fd = open("home.html", O_RDONLY)) < 0) {
            perror("open");
        }

        if ((bytes_read = read(home_html_fd, html_buf, 2000)) < 0) {
            perror("read");
        }

        printf("%s", html_buf);
        fflush(stdout);

        // Adding null terminator
        input_buf[bytes_read+1] = '\0';

        /* TODO: Format buffer into proper format */
        snprintf(output_buf,
                300, 
                "HTTP/1.1 200 OK\nContent-length: %d\nContent-type: text/html\n\n%s", 
                bytes_read + 40, 
                html_buf);

        /* TODO: Send HTML data back to user */
        if (send(conn_fd, output_buf, 2070, 0) < 0) {
            perror("send");
        }

        //printf("%s", output_buf);

        if (close(home_html_fd) < 0) {
            perror("close");
        }

    }

    if (bytes_received < 0) {
        perror("recv");
    }

    if (close(conn_fd) < 0) {
        perror("close");
    }

    return NULL;
}

