#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <errno.h>
#include <string.h>

/* Macro definitions */
#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_err(M, ...) fprintf(stderr, "ERROR: \t(%s:%d: errno: %s) " M "\n",\
                        __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "WARN: \t(%s:%d: errno: %s) " M "\n",\
                        __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "INFO: \t(%s:%d) " M "\n",\
                        __FILE__, __LINE__, ##__VA_ARGS__)

#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", \
                        __FILE__, __LINE__, ##__VA_ARGS__)

#endif
