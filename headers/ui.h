#ifndef LS_UI_H
#define LS_UI_H
#include <stdlib.h>

void ui_init(size_t status_n, size_t warning_n, size_t notice_n); 
// These functions make a copy of the string passed to them.
void update_status_msg(size_t i, char* msg);
void log_warning(char* msg);
void log_notice(char* msg);
void redraw();

#endif
