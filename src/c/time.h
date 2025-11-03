#pragma once

extern unsigned long now;
extern char s_time[];
extern char s_day[];
extern char s_mon[];
extern char s_dow[];
extern char s_year[];

typedef void (*time_changed_cb)(bool updateall);

void time_init(time_changed_cb callback);
void time_deinit();
