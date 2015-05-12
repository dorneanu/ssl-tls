#ifndef CONNECTION_H_
#define CONNECTION_H_

#define HTTP_PORT           80
#define BUFFER_SIZE         255
#define MAX_GET_COMMAND     255

int parse_url (char*, char**, char**);
int parse_server_addr (char*, char**, int*);
int parse_proxy(char*, char**, int*, char**, char**);
int http_connect (char*, int); 
int http_get (int, const char*, const char*, const char*, const char*, const char*);
void print_data(int);

#endif
