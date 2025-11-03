#pragma once

extern unsigned long now;
extern char s_time[8];
extern char s_day[3];
extern char s_mon[5];
extern char s_dow[5];
extern char s_year[5];

typedef void (*time_changed_cb)();

void time_init(time_changed_cb callback);
void time_deinit();
