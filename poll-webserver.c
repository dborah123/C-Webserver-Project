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

#define MAX_FDS 20

void
main(int argc, char *argv[]) {

    char *listen_port;
    int listen_fd, new_fd;
    struct addrinfo hints, *res;
    int rc;
    struct sockaddr_in remote_sa;
    uint16_t remote_port;
    socklen_t addrlen;
    char *remote_ip;

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

    /* TODO: Set up poll structure */

    ndfs_t nfds = 0;
    struct pollfd *poll_fds;
    int max_fds = 0, num_fds = 0;

    if ((poll_fds = malloc(MAX_FDS * sizeof(pollfd))) == NULL) {
        perror("malloc");
    }

    poll_fds->fd = listen_fd;
    poll_fds->events = POLLIN;
    poll_fds->revents = 0;
    num_fds += 1

    /* TODO: Enter while loop */
    while (1) {

        /*TODO: Poll */
        nfds = num_fds;
        if (poll(nfds, poll_fds, -1) == -1) {
            perror("poll");
        }

        /* TODO: Iterate thru file descriptors */
        for (int fd = 0; fd < (nfds+1); fd++) {
            // Skip dead fds
            if ((poll_fds + fd)->fd < 0) {
                continue;
            }

            // Check is fd is ready for reading
            if (((poll_fds + fd)->events & POLLIN) == POLLIN) {
                // Check if fd is a new connection
                if ((poll_fds + fd)->fd == listed_fd) {
                    new_fd = accept(listen_fd, (struct sockaddr *) &remote_sa, &addrlen);

                    if (new_fd < 0) {
                        perror("accept");
                        continue;
                    }

                    // Add new_fd to poll_fds
                    if (num_fds == max_fds) { // Add more space
                        poll_fds = realloc(poll_fds, (max_fds+num_fds) * sizeof(pollfd));
                        if (poll_fds == NULL ) {
                            perror("realloc");
                        }
                    }

                    (poll_fds + num_fds)->fd = new_fd;
                    (poll_fds + num_fds)->events = POLLIN;
                    (poll_fds + num_fds)->revents = 0;


                    /* Announcing new communication partner */
                    remote_ip = inet_ntoa(remote_sa.sin_addr);
                    remote_port = ntohs(remote_sa.sin_port);

                    printf("Connection from %s: %d\n", remote_ip, remote_port);
                }
            } else {
                // TODO: Receive data, process request, and push HTML to connection
            }
        }
    }
}

