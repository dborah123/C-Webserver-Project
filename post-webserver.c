/*
 * post-webserver.c
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
#define BUF_SIZE 3000

void  push_data(char *page, int conn_fd);
void  route_get(char *input_buf, int new_fd);
void  route_post();

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

                    printf("*************************************\n\n");
                    printf("Connection from %s: %d\n", remote_ip, remote_port);
                
                    // Receive data, process request, and push HTML to connection
                    bytes_received = recv(new_fd, input_buf, BUF_SIZE, 0);

                    if (bytes_received < 0) {
                        perror("recv");
                    }

                    printf("\nINCOMING DATA:\n\n%s\n\n", input_buf);

                    if (strncmp(input_buf, "GET", 3) == 0) {
                        route_get(input_buf, new_fd);
                    } else if (strncmp(input_buf, "POST", 4) == 0) {
                  //      route_post();
                    } else {
                        push_data("404-not-found.html", new_fd);
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
route_get(char *input_buf, int conn_fd) {
    /*
     * Routes GET requests to function that pushes correct return type
     */

    /* Route url and send correct page to user */
    if (strncmp(input_buf, "GET /home ", 10) == 0) {
        push_data("home.html", conn_fd);
    } else if (strncmp(input_buf, "GET /about ", 11) == 0) {
        push_data("about.html", conn_fd);
    } else if (strncmp(input_buf, "GET /style.css", 14) == 0) {
        push_data("style.css", conn_fd);
    } else if (strncmp(input_buf, "GET /sign-up", 12) == 0) {
        push_data("sign-up.html", conn_fd);
    } else if (strncmp(input_buf, "GET /index.js", 13) == 0) {
        push_data("index.js", conn_fd);
    } else if (strncmp(input_buf, "GET /home.js", 12) == 0) {
        push_data("home.js", conn_fd);
    } else {
        push_data("404-not-found.html", conn_fd);
    }
}


void
route_post(char *input_buf){
    /**
     * Routes POST requests to correct function
     */
    if (strncmp(input_buf, "POST /sign-up", 13) == 0) {
        //handlePost()        
    } else {
        //push_data("404-not-found.html", conn_fd)
    }
}


void
push_data(char *page, int conn_fd) {
    /**
     * Processes request for specific page and sents it to client
     */
    int bytes_read;
    int html_fd;
    int bytes_sending;

    char *file_type;
    char *content_type;
    int content_type_len;

    char html_buf[BUF_SIZE];
    char output_buf[BUF_SIZE];

    /* Retreive specific text file and read it into buffer */
    if ((html_fd = open(page, O_RDONLY)) < 0) {
        perror("open");
    }

    if ((bytes_read = read(html_fd, html_buf, BUF_SIZE)) < 0) {
        perror("read");
    }

    /* Set content type */
    file_type = (strchr(page, '.') + 1);

    if (strcmp(file_type, "html") == 0){
        content_type = "text/html";
        content_type_len = strlen(content_type);
    } else if (strcmp(file_type, "css") == 0) {
        content_type = "text/css";
        content_type_len = strlen(content_type);
    } else {
        perror("content type");
    }

    /* Format buffer into proper format */
    if (strcmp(page, "404-not-found.html") == 0) {
        bytes_sending = bytes_read + content_type_len + 67;
        snprintf(output_buf,
            bytes_sending,
            "HTTP/1.1 404 Not Found\nContent-length: %d\nContent-type: %s\n\n%s",
            bytes_read,
            content_type,
            html_buf);
    } else {
        bytes_sending = bytes_read + content_type_len + 60;
        snprintf(output_buf,
            bytes_sending,
            "HTTP/1.1 200 OK\nContent-length: %d\nContent-type: %s\n\n%s",
            bytes_read,
            content_type,
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


void
handlePost() {
    /**
     * Parses POST request, prints them, and stores them if necessary
     */

    // NEED PARSER

    /* TODO: Somehow only look at content of request
     *       Print personal info in server
     *       Save message in a buffer
     */


    /*
     * New idea: create a parser myself and store each request in a struct
     */
}

