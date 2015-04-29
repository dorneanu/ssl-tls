#ifndef SSL_TLS_H_
#define SSL_TLS_H_

typedef struct globalargs {
    char *prog;
    char *url;
    char *proxy_uri;
    int port;
    int verbose;
    int debug;

    /* Proxy settings */
    char *proxy_username;
    char *proxy_password;
    char *proxy_host;
    int  proxy_port;

} globalargs;

#endif
