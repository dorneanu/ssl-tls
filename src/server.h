#ifndef SERVER_H_
#define SERVER_H_

#define MAX_CONN 10      // how many pending connections queue will hold
void *get_in_addr(struct sockaddr *);
int http_listen(char *, int);

#endif
