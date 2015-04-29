/*
 * main.c
 * Copyright (C) 2015 victor <victor@Delia>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdio.h>
#include <stdlib.h>
#include "ssl-tls.h"
#include "log.h"
#include "args.h"
#include "connection.h"

/* Function declarations */
void print_help(char *);

/* Function definitions */
void print_help(char *m) {
    printf("Usage: %s: -u <URL> [-p http://[username:password@]proxy-host:proxy-port]\n", m);
}

/* Main function */
int main(int argc, char **argv) {
    int opts, client_socket;
    char *host, *path, *proxy;
    host = path = proxy = NULL;

    // Global arguments
    globalargs *args;

    // Allocate memory
    if (!(args = (globalargs *)malloc(sizeof(globalargs)))) {
        log_err("Sth went wrong with malloc!");
    }

    // Set default options
    args->prog = argv[0];
    args->verbose = 0;
    args->debug = 0;
    args->proxy_host = NULL;
    args->proxy_username = NULL;
    args->proxy_password = NULL;

    // Get options
    getoptions(argc, argv, args);

    // Check options
    if ((opts = checkoptions(args))) {
        log_info("Alles gut");

        // Parse URL
        if (parse_url(args->url, &host, &path) == -1) {
            log_err("Failed parsing URL");
            exit(1);
        }

        // Parse proxy (if specified)
        if (args->proxy_uri) {
            parse_proxy(
                args->proxy_uri, 
                &args->proxy_host, &args->proxy_port, 
                &args->proxy_username, &args->proxy_password
            );
            
            if (args->debug) {
                log_info("Proxy Host: %s", args->proxy_host);
                log_info("Proxy port: %d", args->proxy_port);
                log_info("Proxy Username: %s", args->proxy_username);
                log_info("Proxy Password: %s", args->proxy_password);
            }
        }

        if(args->proxy_host) {              // connect to proxy
            if((client_socket = http_connect(args->proxy_host, *(&(args->proxy_port)))) < 0) {
                log_err("Failed connecting to proxy!");
                return -1;
            }
        } else {                            // connect to host directly
            if((client_socket = http_connect(host, 80)) < 0) {
                log_err("Failed connecting to host!");
                return -1;
            }
        }

        // Send GET-Request
        http_get(client_socket, path, host, args->proxy_host, args->proxy_username, args->proxy_password);

        // Get and print response
        print_data(client_socket);
    }
    else {
        log_err("Parameter check failed!");
    }
    
    return 0;
}
