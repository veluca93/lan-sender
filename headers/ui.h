#ifndef LS_UI_H
#define LS_UI_H
#include <string.h>
#include <stdlib.h>
#define ERRLEN 256
#define PRINT_IF_ERROR(cond, string, status) if ((cond) == -1) { \
    fprintf(stderr, string ": %s", strerror(errno)); \
    if (status) exit(status); \
}

#define LOG_IF_ERROR(cond, string, ret) if ((cond) == -1) { \
    char* str = malloc(strlen(string) + ERRLEN + 2); \
    strcpy(str, string); \
    strcat(str+strlen(string), ": "); \
    strerror_r(errno, str + strlen(string) + 2, ERRLEN); \
    log_warning(str); \
    free(str); \
    return ret; \
}

void ui_init(size_t status_n, size_t warning_n, size_t notice_n); 
// These functions make a copy of the string passed to them.
void update_status_msg(size_t i, char* msg);
void log_warning(char* msg);
void log_notice(char* msg);
void redraw();

#endif
