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

#define BUF_SIZE 2000
#define BACKLOG 10

void *service_connection(void *data);
void  push_page(char *page, int conn_fd);

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
    int *conn_fd_heap;

    // Placeholder
    pthread_t thread;

    listen_port = argv[1];

    /* create a socket */
    if ((listen_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    /* bind it to a port */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if((rc = getaddrinfo(NULL, listen_port, &hints, &res)) != 0) {
        printf("getaddrinfo failed: %s\n", gai_strerror(rc));
        exit(2);
    }

    if (bind(listen_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind");
        exit(3);
    }

    /* start listening */
    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen");
        exit(4);
    }

    printf("Threaded HTTP Server waiting on port %s\n", listen_port);

    addrlen = sizeof(remote_sa);

    /* Accept new connections */
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
            exit(5);
        }

        *conn_fd_heap = conn_fd;

        /* Announcing new communication partner */
        remote_ip = inet_ntoa(remote_sa.sin_addr);
        remote_port = ntohs(remote_sa.sin_port);

        printf("Connection from %s: %d\n", remote_ip, remote_port);

        /* Spinning a new thread to service connection */
        if (pthread_create(&thread, NULL, service_connection, conn_fd_heap) != 0) {
            perror("pthread_create");
            exit(6);
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
    char input_buf[BUF_SIZE];

    /* Receive data from client */
    if ((bytes_received = recv(conn_fd, input_buf, BUF_SIZE, 0)) < 0) {
        perror("recv");        
        exit(7);
    }
    /* Parse data to figure out which HTML file one wants. Throw error is not found */

    if (strncmp(input_buf, "GET /home ", 10) == 0) {
        push_page("home.html", conn_fd);
    } else if (strncmp(input_buf, "GET /about ", 11) == 0) {
       push_page("about.html", conn_fd);
    } else {
       push_page("404-not-found.html", conn_fd);
    }

    if (close(conn_fd) < 0) {
        perror("close");
        exit(8);
    }

    return NULL;
}


void 
push_page(char *page, int conn_fd) {
    /**
     * Processes request for specific page and sents it to client
     */

    int bytes_read;
    int html_fd;
    int bytes_sending;
    char html_buf[BUF_SIZE];
    char output_buf[BUF_SIZE];

    /*  Retreive specific HTML FILE and read it into buffer */
    if ((html_fd = open(page, O_RDONLY)) < 0) {
        perror("open");
        exit(9);
    }

    if ((bytes_read = read(html_fd, html_buf, 2000)) < 0) {
        perror("read");
        exit(10);
    }

    /* Format buffer */
    if (strcmp(page, "404-not-found.html") == 0) {
        bytes_sending = bytes_read + 67;
        snprintf(output_buf,
            bytes_sending, 
            "HTTP/1.1 404 Not Found\nContent-length: %d\nContent-type: text/html\n\n%s", 
            bytes_read, 
            html_buf);
    } else {
        bytes_sending = bytes_read + 60;
        snprintf(output_buf,
            bytes_sending, 
            "HTTP/1.1 200 OK\nContent-length: %d\nContent-type: text/html\n\n%s", 
            bytes_read, 
            html_buf);
    }

    /* Send HTML data back to user */
    if (send(conn_fd, output_buf, bytes_sending, 0) < 0) {
        perror("send");
        exit(11);
    }

    if (close(html_fd) < 0) {
        perror("close");
        exit(12);
    }
}

