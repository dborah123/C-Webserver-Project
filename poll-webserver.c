/*
 * poll-webserver.c
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
#include <poll.h>
#include <fcntl.h>

#define BACKLOG 20
#define MAX_FDS 20
#define BUF_SIZE 4000

void  push_page(char *page, int conn_fd);

int
main(int argc, char *argv[]) {

    char *listen_port;
    int listen_fd, new_fd;
    struct addrinfo hints, *res;
    int rc;
    struct sockaddr_in remote_sa;
    uint16_t remote_port;
    socklen_t addrlen;
    char *remote_ip;

    int bytes_received;
    char input_buf[BUF_SIZE];

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

    /* Set up poll structure */

    nfds_t nfds = 0;
    struct pollfd *poll_fds;
    int  num_fds = 0;

    if ((poll_fds = malloc(MAX_FDS * sizeof(struct pollfd))) == NULL) {
        perror("malloc");
    }

    poll_fds->fd = listen_fd;
    poll_fds->events = POLLIN;
    poll_fds->revents = 0;
    num_fds += 1;

    struct pollfd *curr_file;
    while (1) {

        /* Poll blocks until fd is ready for reading */
        nfds = num_fds;
        if (poll(poll_fds, nfds, -1) == -1) {
            perror("poll");
        }

        /* Iterate thru file descriptors 
         * Note: This is probably not necessary as we are closing every new
         * connection before another is accepted
         */
        for (int fd = 0; fd < (nfds+1); fd++) {
            curr_file = (poll_fds + fd);

            // Skip dead fds. Not necessary as we are just using one
            if (curr_file->fd < 0) {
                continue;
            }

            // Check is fd is ready for reading
            if ((curr_file->events & POLLIN) == POLLIN) {
                // Check if fd is a new connection
                if (curr_file->fd == listen_fd) {
                    new_fd = accept(listen_fd, (struct sockaddr *) &remote_sa, &addrlen);

                    if (new_fd < 0) {
                        perror("accept");
                        continue;
                    }

                    /* Announcing new communication partner */
                    remote_ip = inet_ntoa(remote_sa.sin_addr);
                    remote_port = ntohs(remote_sa.sin_port);

                    printf("Connection from %s: %d\n", remote_ip, remote_port);
                
                    // Receive data, process request, and push HTML to connection
                    bytes_received = recv(new_fd, input_buf, BUF_SIZE, 0);

                    if (bytes_received < 0) {
                        perror("recv");
                    }

                    printf("\n*****************************\nINCOMING DATA:\n\n%s\n\n", input_buf);

                    // Route url and send correct page to user
                    if (strncmp(input_buf, "GET /home ", 10) == 0) {
                        push_page("home.html", new_fd);
                    } else if (strncmp(input_buf, "GET /about ", 11) == 0) {
                        push_page("about.html", new_fd);
                    } else {
                        push_page("404-not-found.html", new_fd);
                    }

                    // Terminate connection with client
                    printf("Client at socket %d closed\n", new_fd);

                    if (close(new_fd) != 0) {
                        perror("close");
                    }
                }
            }
        }
    }
    exit(EXIT_SUCCESS);
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

    /* Retreive specific HTML file and read it into buffer */
    if ((html_fd = open(page, O_RDONLY)) < 0) {
        perror("open");
    }

    if ((bytes_read = read(html_fd, html_buf, BUF_SIZE)) < 0) {
        perror("read");
    }

    /* Format buffer into proper format */
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
    }

    if (close(html_fd) < 0) {
        perror("close");
    }
}

