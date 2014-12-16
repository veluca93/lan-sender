#define _DEFAULT_SOURCE
#include "ui.h"
#include <unistd.h>
#include <stdio.h>

int main() {
    ui_init(3, 10, 3);
    update_status_msg(0, "Status 0");
    update_status_msg(1, "Status 1");
    update_status_msg(2, "Status 2");
    log_warning("LOL");
    log_notice("asd");
    redraw();
    for (size_t i=0; i<100; i++) {
        usleep(100000);
        char tmp[1000];
        sprintf(tmp, "LOL %lu", i);
        log_warning(tmp);
        sprintf(tmp, "Status 0: %lu", i);
        update_status_msg(0, tmp);
        redraw();
    }
}
