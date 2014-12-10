#include "hash.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char** argv) {
    int i;
    sha224_t hash;
    sha224_ctx_t* ctx = sha224_init();
    uint8_t buff[1024];
    size_t rsize = 0;
    int fd;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }
    fd = open(argv[1], O_RDONLY);
    if (!fd) {
        fprintf(stderr, "Cannot open %s: %s\n", argv[1], strerror(errno));
        return 2;
    }
    do {
        errno = 0;
        rsize = read(fd, buff, 1024);
        if (errno) {
            fprintf(stderr, "Error reading %s: %s\n", argv[1], strerror(errno));
            return 3;
        }
        sha224_update(ctx, buff, rsize);
    } while(rsize==1024);
    sha224_finalize(&hash, ctx);
    for (i=0; i<28; i++) printf("%02x", hash.hash[i]);
    printf(" %s\n", argv[1]);
    return 0;
}
