#ifndef LS_COMMUNICATION_H
#define LS_COMMUNICATION_H
#include <stdlib.h>
#include <sys/socket.h>

typedef void (*accept_handler)(int peerno);
typedef void (*connected_handler)(int peerno);
typedef void (*msg_handler)(int peerno, int msg);
typedef void (*close_handler)(int perno);

void communication_init(const char* host, const char* port);
void communication_start();
void communication_connect(connected_handler hdl, const char* host, const char* port);
void register_accept_handler(accept_handler hdl);
void register_msg_handler(int msg, msg_handler hdl);
void register_close_handler(close_handler hdl);
int send_to_peer(int peerno, const void* buf, size_t len);
int read_from_peer(int peerno, void* buf, size_t len);

#endif
