#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include "ssl-tls.h"
#include "connection.h"
#include "log.h"

void print_usage (globalargs *args) {
    printf("Usage: %s <additional options> -p <proxy> -u <URL>\n", args->prog);
    printf("Additional options:\n");
    printf("\t -v (--verbose)\t\tAdd verbosity\n");
    printf("\t -D (--debug)\t\tEnable debug messages\n");
    exit(EXIT_SUCCESS);
}

void getoptions (int argc, char **argv, globalargs *globalargs) {
    static const char *optstring = "vDh?p:u:l:";

    static const struct option longopts[] = {
        { "help",       no_argument,        NULL,   'h'},
        { "Debug",      no_argument,        NULL,   'D'},
        { "verbose",    no_argument,        NULL,   'v'},
        { "listen",     required_argument,  NULL,   'l'},
        { "proxy",      required_argument,  NULL,   'p'},
        { "url",        required_argument,  NULL,   'u'},
        { NULL,         no_argument,        NULL,    0 }
    };

    int opt = 0;
    int longindex = 0;
    
    /* Process the arguments with getopt_long(), then populate globalargs-> */
    opt = getopt_long( argc, argv, optstring, longopts, &longindex );
    while( opt != -1 ) {
        switch( opt ) {
            case '?':
                print_usage(globalargs);
                break;
            case 'h':
                print_usage(globalargs);
                break;
            case 'D':
                globalargs->debug = 1;
                break;
            case 'v':
                globalargs->verbose++;
                break;
            case 'l':
                globalargs->server_addr = optarg;
                break;
            case 'p':
                globalargs->proxy_uri =  optarg;
                break;
            case 'u':
                globalargs->url = optarg;
                break;
            case 0:                 /* long option without a short arg */
                if( strcmp( "debug", longopts[longindex].name ) == 0 ) {
                    globalargs->debug = 1;
                }
                if( strcmp( "verbose", longopts[longindex].name ) == 0 ) {
                    globalargs->verbose = 1;
                }
                if( strcmp( "proxy", longopts[longindex].name ) == 0 ) {
                    globalargs->proxy_uri = optarg;
                }
                if( strcmp( "url", longopts[longindex].name ) == 0 ) {
                    globalargs->url = optarg;
                }
                break;
            default:
                break;
        }
        opt = getopt_long( argc, argv, optstring, longopts, &longindex );
    }
}

int checkoptions (globalargs *args) {
    // Check options
    if (args->debug) {
        debug("Proxy: %s", args->proxy_uri);
        debug("URL: %s", args->url);
    }

    // Check required options
    if (!args->url) {
        log_err("No URL specified.");
        return 0;
    }

    return 1;
}
