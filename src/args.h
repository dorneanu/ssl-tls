#ifndef ARGS_H_
#define ARGS_H_

#include "ssl-tls.h"

void getoptions (int, char**, globalargs*);
int checkoptions (globalargs *); 
void print_usage (globalargs*);


#endif
