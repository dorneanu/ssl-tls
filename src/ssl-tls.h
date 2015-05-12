#ifndef SSL_TLS_H_
#define SSL_TLS_H_

typedef struct globalargs {
    /* General settings */
    char *prog;
    int port;
    int verbose;
    int debug;
    
    /* GET-Request settings */
    char *url;
    char *proxy_uri;

    /* Server settings */
    char *server_addr;
    char *server_ip;
    int server_port;

    /* Proxy settings */
    char *proxy_username;
    char *proxy_password;
    char *proxy_host;
    int  proxy_port;

} globalargs;

#endif
