#include "communication.h"
#include "ui.h"
#include <sys/resource.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

// Handlers
static accept_handler ah;
static close_handler ch;
static msg_handler mh[256];
static msg_handler mh_default;
static connected_handler* cnh;

// Maximum number of connections
static int fdmax;

// Socket fd <---> peer number
static int* peerno;
static int* sfd;

// Threads and thread's buffers array
static pthread_t* threads;
static void* (*buffers)[64];
static size_t (*buffsize)[64];
static int* nbuffs;
static pthread_mutex_t* buff_mutexes;

// Listening socket
static int srvsocket;

void read_or_write(int sock) {
    while (1) {
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 200;
        fd_set fd;
        FD_ZERO(&fd);
        FD_SET(sock, &fd);
        int ret;
        LOG_IF_ERROR(ret = select(FD_SETSIZE, &fd, NULL, NULL, &timeout), "Select",);
        if (ret == 0) {
            // Timeout, eg. nothing to read
            pthread_mutex_lock(buff_mutexes+sock);
            nbuffs[sock]--;
            send_to_peer(peerno[sock], buffers[sock][nbuffs[sock]], buffsize[sock][nbuffs[sock]]);
            free(buffers[sock][nbuffs[sock]]);
            pthread_mutex_unlock(buff_mutexes+sock);
        } else {
            // Something to read: read the message number, then call the appropriate handler.
            char msgno_;
            LOG_IF_ERROR(recv(sock, &msgno_, 1, MSG_PEEK), "Read",);
            int msgno = msgno_;
            if (mh[msgno]) mh[msgno](peerno[sock], msgno);
            else mh_default(peerno[sock], msgno);
        }
    }
}

void* accepter(void* data) {
    int sock = (int) (size_t) data;
    ah(peerno[sock]);
    read_or_write(sock);
    ch(peerno[sock]);
    return NULL;
}

void* listener(void* data) {
    int sock;
    LOG_IF_ERROR(listen(srvsocket, 5), "Failed to listen", NULL);
    while (1) {
        LOG_IF_ERROR(sock = accept(srvsocket, NULL, NULL), "Accept failed", NULL);
        pthread_create(threads+sock, NULL, &accepter, (void*) (size_t) sock);
    }
    return NULL;
}

void* connecter(void* data) {
    int sock = (int) (size_t) data;
    cnh[sock](peerno[sock]);
    read_or_write(sock);
    ch(peerno[sock]);
    close(sock);
    return NULL;
}

void communication_end() {
    size_t i;
    for (i=0; i<fdmax; i++) pthread_join(threads[i], NULL);
    close(srvsocket);
}

void communication_init(const struct sockaddr* addr, socklen_t addrlen) {
    struct rlimit fdno;
    getrlimit(RLIMIT_NOFILE, &fdno);
    fdmax = fdno.rlim_cur;
    peerno = calloc(fdmax, sizeof* peerno);
    sfd = calloc(fdmax, sizeof* sfd);
    threads = calloc(fdmax, sizeof* threads);
    cnh = calloc(fdmax, sizeof* cnh);
    buffers = calloc(fdmax, sizeof* buffers);
    buffsize = calloc(fdmax, sizeof* buffsize);
    nbuffs = calloc(fdmax, sizeof* nbuffs);
    buff_mutexes = calloc(fdmax, sizeof* buff_mutexes);
    size_t i;
    for (i=0; i<fdmax; i++)
        pthread_mutex_init(buff_mutexes + i, NULL);
    PRINT_IF_ERROR(srvsocket = socket(AF_INET, SOCK_STREAM, 0), "Can't create server socket", 2);
    PRINT_IF_ERROR(bind(srvsocket, addr, addrlen), "Can't bind server socket", 2);
    atexit(&communication_end);
}

void communication_start() {
    pthread_create(threads+srvsocket, NULL, &listener, NULL);
}

void communication_connect(connected_handler hdl, const struct sockaddr* addr, socklen_t addrlen) {
    int sock;
    LOG_IF_ERROR(sock = socket(AF_INET, SOCK_STREAM, 0), "Can't create client socket",);
    LOG_IF_ERROR(bind(sock, addr, addrlen), "Can't bind client socket",);
    cnh[sock] = hdl;
    pthread_create(threads+sock, NULL, &connecter, (void*) (size_t) sock);
}

void register_accept_handler(accept_handler hdl) {
    ah = hdl;
}

void register_msg_handler(int msg, msg_handler hdl) {
    if (msg > -1) mh[msg] = hdl;
    else mh_default = hdl;
}

void register_close_handler(close_handler hdl) {
    ch = hdl;
}

int send_to_peer(int peerno, const void* buf_, size_t len) {
    const char* buf = buf_;
    while (len > 0) {
        int ret = send(sfd[peerno], buf, len, 0);
        if (ret < 1 && errno != EINTR) return 0;
        len -= ret;
        buf += ret;
    }
    return 1;
}

int read_from_peer(int peerno, void* buf_, size_t len) {
    char* buf = buf_;
    while (len > 0) {
        int ret = recv(sfd[peerno], buf, len, 0);
        if (ret < 1 && errno != EINTR) return 0;
        len -= ret;
        buf += ret;
    }
    return 1;
}

void sendlater(int peerno, void* msg, size_t size) {
    int sock = sfd[peerno];
    pthread_mutex_lock(buff_mutexes+sock);
    buffers[sock][nbuffs[sock]] = memcpy(malloc(size), msg, size);
    buffsize[sock][nbuffs[sock]] = size;
    nbuffs[sock]++;
    pthread_mutex_unlock(buff_mutexes+sock);
}
