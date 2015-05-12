#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include "server.h"
#include "log.h"


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    } else {
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}

int http_listen (char *server_hostname, int server_port) {
    int listen_sockfd, connection_sockfd, status, yes=1;
    char port[20];
    socklen_t sin_size;
    struct sockaddr_in *server;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage client_addr; // client's address information
    char client[INET6_ADDRSTRLEN];
    
    // Create structs
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE; // use my ip
   
    // Check for port
    if (server_port)
        sprintf(port, "%d", server_port);

    // Call getaddrinfo
    if ((status = getaddrinfo(server_hostname, port, &hints, &res)) != 0) {
        log_err("getaddrinfo: %s", gai_strerror(status));
    }
    
    // Loop through all results and bind to the first we can
    for (p = res; p != NULL; p->ai_next) {

        // Create socket
        if ((listen_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            log_err("Failed creating socket!");
        }

        // Set socket options
        if (setsockopt(listen_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            close(listen_sockfd);
            log_err("Failed setting socket options!");
            continue;
        }

        // Bind to socket
        if (bind(listen_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(listen_sockfd);
            log_err("Failed to bind socket!");
            continue;
        }

        // Success!
        break;
    }
    
    // Free structs
    freeaddrinfo(res);

    // Listen 
    if (listen(listen_sockfd, MAX_CONN) == -1) {
        log_err("Failed to listen!");
        return -2;
    }

    // Listen for incoming connections
    while ((connection_sockfd = accept(
            listen_sockfd, (struct sockaddr *)&client_addr,
            &sin_size)) != -1)
    {
      
        // Convert to network to PC
        inet_ntop(
            client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr),
            client, sizeof(client)
        );

        log_info("Got connection from: %s", client);

        // Process request
        // process_request(connection_sockfd);
    }
}
