#ifndef SERVER_H_
#define SERVER_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#define MAX_CONN 10      // how many pending connections queue will hold
#define DEFAULT_LINE_LEN 255

void *get_in_addr(struct sockaddr *);
int http_listen(char *, int);
void send_success_response (int);
void send_error_response (int, int);
void process_request (int);
char *read_line (int);

#endif
