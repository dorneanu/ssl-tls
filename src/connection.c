#include <string.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include "connection.h"
#include "log.h"
#include "utils/base64.h"

int parse_url (char *uri, char **host, char **path) {
    char *pos;
    if(!(pos = strchr(uri, '/'))) {
        log_err("Malformed URL");
        return -1;
    }

    // Set host
    *host = pos + 2;

    // Find end of host
    if(!(pos = strchr(*host, '/'))) {
        log_err("Malformed URL");
        return -1;
    }
    else {
        *pos = '\0';
        *path = pos +1;
    }
    return 0;
}

int parse_proxy (char *proxy_spec, char **proxy_host, int *proxy_port,
                 char **proxy_user, char **proxy_password) {
    char *login_sep, *colon_sep, *trailer_sep;
    // Check for 'http://' at the beginning
    if (!strncmp("http://", proxy_spec, 7)) {
        proxy_spec += 7;
    }

    // Check for username:password
    if ((login_sep = strchr(proxy_spec, '@'))) {
        colon_sep = strchr(proxy_spec, ':');

        if (!colon_sep || (colon_sep > login_sep)){
            log_err("Expected password in %s", proxy_spec);
            return 0;
        }

        *colon_sep = '\0';
        *proxy_user = proxy_spec;

        *login_sep = '\0';
        *proxy_password = colon_sep + 1;
        proxy_spec = login_sep + 1;
    
        // Ignore '/' at the end
        if ((trailer_sep = strchr(proxy_spec, '/'))) {
            *trailer_sep = '\0';
        }

        // Check for port
        if ((colon_sep = strchr(proxy_spec, ':'))) {
            *colon_sep = '\0';
            *proxy_host = proxy_spec;
            *proxy_port = atoi(colon_sep + 1);
        }
        else {                  // was not specified
            *proxy_port = HTTP_PORT;
            *proxy_host = proxy_spec;
        }
    }

    return 1; 
}

int parse_server_addr(char *server_addr, char **server_hostname, int *server_port)
{
    char *hostname_sep, *colon_sep;

    // Check for ":" in "<hostname>:<port>" string
    if ((hostname_sep = strchr(server_addr, ':'))) {
        *hostname_sep = '\0'; 
        *server_hostname = server_addr;
        *server_port = atoi(hostname_sep + 1);
        log_info("Hostname: %s", *server_hostname);
        log_info("Port: %d", *server_port);

    }

    return 1;
}

int http_connect (char *server_hostname, int server_port) {
    int socketfd, status;
    char port[20]="80";
    char ipstr[INET6_ADDRSTRLEN];
    struct sockaddr_in *remote;
    struct addrinfo hints, *res;
    void *addr;

    // Create structs
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Check for port
    if (server_port) 
        sprintf(port, "%d", server_port);

    // Getaddrinfo for server hostname
    if ((status = getaddrinfo(server_hostname, port, &hints, &res)) != 0) {
        log_err("getaddrinfo: %s", gai_strerror(status));
        return -2;
    }

    /*
    for (p = res; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;
        
        struct sockaddr_in *remote = (struct sockaddr_in *)p->ai_addr;
        addr = &(remote->sin_addr);

        // Convert IP to stringn and print it
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("%s\n", ipstr);
    }
    */

    // Take first result
    remote = (struct sockaddr_in *)res->ai_addr;
    addr = &(remote->sin_addr);

    // Convert IP to string
    inet_ntop(res->ai_family, addr, ipstr, sizeof(ipstr));

    // Create socket
    if ((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        log_err("Failed creating socket");
        return -1;
    }
   
    log_info("Connecting to %s:%s", ipstr, port);

    // Connect to host/proxy
    if (connect(socketfd, res->ai_addr, res->ai_addrlen) == -1) {
        log_err("Connect failed");
        return -3;
    }

    // Free linked-list
    freeaddrinfo(res);

    // Return socket
    return socketfd;
}


int http_get(int connection, const char *path, const char *host,
        const char *proxy_host,
        const char *proxy_user,
        const char *proxy_password) 
{
    static char get_command[MAX_GET_COMMAND];

    if (proxy_host) {
        sprintf( get_command, "GET http://%s/%s HTTP/1.1\r\n", host, path );
    } else {
        sprintf( get_command, "GET /%s HTTP/1.1\r\n", path );
    }

    // Send GET command
    if (send(connection, get_command, strlen(get_command), 0) == -1)
    return -1;

    // Send Host header
    sprintf(get_command, "Host: %s\r\n", host);
    if (send(connection, get_command, strlen(get_command), 0) == -1)
        return -1;

    // Authentificate to proxy
    if (proxy_user)
    {
        int credentials_len = strlen(proxy_user) + strlen(proxy_password) + 1;
        char *proxy_credentials = malloc(credentials_len);
        unsigned char *auth_string = malloc(((credentials_len * 4) / 3) + 1);
        size_t auth_string_len = sizeof(auth_string);
        sprintf(proxy_credentials, "%s:%s", proxy_user, proxy_password);

        // Base64 encode credentials
        base64_encode(auth_string, &auth_string_len, (unsigned char *)proxy_credentials, credentials_len);
        sprintf( get_command, "Proxy-Authorization: BASIC %s\r\n", auth_string );

        // Send Basic-Auth header
        if (send(connection, get_command, strlen( get_command), 0) == -1) {
            free(proxy_credentials);
            free(auth_string);
            return -1;
        }

        free( proxy_credentials );
        free( auth_string );
    }


    // Send Close header
    sprintf(get_command, "Connection: close\r\n\r\n");
    if (send( connection, get_command, strlen(get_command), 0) == -1)
        return -1;

    return 0;
}

void print_data(int connection) { 
    int received = 0;
    static char recv_buffer[BUFFER_SIZE + 1];

    // Get some data
    printf("-----------\n");
    while ((received = recv(connection, recv_buffer, BUFFER_SIZE, 0)) > 0) {
        recv_buffer[received] = '\0';
        printf("%s", recv_buffer);
    }
    printf("-----------\n");
}
