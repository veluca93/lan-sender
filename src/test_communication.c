#include "communication.h"
#include "ui.h"
#include <unistd.h>
#include <stdio.h>

void mhandler(int peerno, int msg) {
    char _;
    read_from_peer(peerno, &_, 1);
    char buf[1024];
    int n = sprintf(buf, "Received message %d\n", msg);
    log_warning(buf);
    send_to_peer(peerno, buf, n);
}

int main() {
    ui_init(4, 10, 0);
    communication_init("0.0.0.0", "1234");
    register_msg_handler(-1, &mhandler);
    communication_start();
    while (1) {
        usleep(100);
        redraw();
    }
}
