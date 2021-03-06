#ifndef LS_MESSAGES_H
#define LS_MESSAGES_H

#include <stdint.h>
#include "hash.h"
#include "net.h"

// Successful messages
#define ALLFINISHED         0
#define GETFILES            1
#define GETFILEPARTS        2
#define READY               3
#define HASHASH             4
#define LISTHASHES          5
#define GETPARTFROM         6
#define SENDPARTTO          7

// Error conditions
#define UNKNOWNMESSAGE      0
#define FILESCHANGED        1
#define PARTRECEIVEERROR    2
#define CONNECTIONERROR     3
#define UNKNOWNHASH         4

void sendsimplemsg(int peerno, int msgno);

typedef struct msg_info {
    uint8_t error : 1;
    uint8_t msgtype : 7;
} msg_info_t;

typedef struct __attribute__ ((__packed__)) file {
    hash_t hash;
    uint64_t size;
    uint8_t namelength;
    char name[];
} file_t;

typedef struct __attribute__ ((__packed__)) msg_getfiles {
    msg_info_t info;
    uint8_t taglenght;
    char tag[];
} msg_getfiles_t;

typedef struct __attribute__ ((__packed__)) msg_getfiles_ans {
    msg_info_t info;
    uint32_t totalparts;
    uint32_t nfiles;
} msg_getfiles_ans_t;

file_t* askfiles(int peerno, char* tag, int* nfiles);

typedef struct __attribute__ ((__packed__)) msg_getfileparts {
    msg_info_t info;
    hash_t hash;
} msg_getfileparts_t;

typedef struct __attribute__ ((__packed__)) msg_getfileparts_ans {
    msg_info_t info;
    uint32_t nparts;
} msg_getfileparts_ans_t;

hash_t* askfileparts(int peerno, hash_t hash, int* nparts);

typedef struct __attribute__ ((__packed__)) msg_hashash {
    msg_info_t info;
    hash_t hash;
} msg_hashash_t;

int hashash(int peerno, hash_t hash);

typedef struct __attribute__ ((__packed__)) msg_listhashes_ans {
    msg_info_t info;
    uint32_t nhashes;
} msg_listhashes_ans_t;

hash_t* listhashes(int peerno, int* nhashes);

typedef struct __attribute__ ((__packed__)) msg_getpartfrom {
    msg_info_t info;
    hash_t hash;
    ip_t ip;
} msg_getpartfrom_t;

typedef struct __attribute__ ((__packed__)) msg_sendpartto {
    msg_info_t info;
    hash_t hash;
    ip_t ip;
} msg_sendpartto_t;

int sendpart(int receiver, int sender, hash_t hash);

#endif
