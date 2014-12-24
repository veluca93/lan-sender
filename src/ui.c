#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

static pthread_mutex_t ui_mutex;
static size_t status_n, warning_n, notice_n;
static char** statuses;
static char** warnings;
static char** notices;

char* mystrndup(const char* str, size_t len) {
    char* ret = malloc(1+len);
    strncpy(ret, str, len);
    ret[len] = 0;
    if (strlen(str) > len)
        ret[len-1] = ret[len-2] = ret[len-3] = '.';
    return ret;
}

void ui_init(size_t status_N, size_t warning_N, size_t notice_N) {
    status_n = status_N;
    warning_n = warning_N;
    notice_n = notice_N;
    statuses = calloc(status_n, sizeof* statuses);
    warnings = calloc(warning_n, sizeof* warnings);
    notices = calloc(notice_n, sizeof* notices);
    puts("\033[2J\033[1;1H");
    pthread_mutex_init(&ui_mutex, NULL);
    if (status_n + warning_n + notice_n > 22)
        log_warning("There might not be enough lines available!");
}

void update_status_msg(size_t i, char* msg) {
    pthread_mutex_lock(&ui_mutex);
    free(statuses[i]);
    statuses[i] = mystrndup(msg, 80);
    pthread_mutex_unlock(&ui_mutex);
}

void log_warning(char* msg) {
    size_t i;
    pthread_mutex_lock(&ui_mutex);
    free(warnings[warning_n-1]);
    if (warning_n)
        for (i=warning_n-1; i>0; i--)
            warnings[i] = warnings[i-1];
    warnings[0] = mystrndup(msg, 80);
    pthread_mutex_unlock(&ui_mutex);
}

void log_notice(char* msg) {
    pthread_mutex_lock(&ui_mutex);
    free(notices[notice_n-1]);
    size_t i;
    if (notice_n)
        for (i=notice_n-1; i>0; i--)
            notices[i] = notices[i-1];
    notices[0] = mystrndup(msg, 80);
    pthread_mutex_unlock(&ui_mutex);
}

void redraw() {
    pthread_mutex_lock(&ui_mutex);
    size_t i;
    puts("\033[2J\033[1;1H");
    for (i=0; i<status_n; i++)
        printf("%s\n", statuses[i]?statuses[i]:"");
    for (i=0; i<warning_n; i++)
        printf("%s\n", warnings[i]?warnings[i]:"");
    for (i=0; i<notice_n; i++)
        printf("%s\n", notices[i]?notices[i]:"");
    pthread_mutex_unlock(&ui_mutex);
}
