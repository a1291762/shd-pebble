#include <pebble.h>
#include "settings.h"
#include "time.h"

unsigned long now = 0;
char s_time[8];
char s_day[3];
char s_mon[5];
char s_dow[5];
char s_year[5];
static time_changed_cb time_changed;

static void update_time(bool force) {
    time_t temp = time(NULL);
    now = temp * 1000;

    if (force || temp % 60 == 0) {
        // once per minute is fine!
        struct tm *tick_time = localtime(&temp);
        strftime(s_time, sizeof(s_time), (clock_is_24h_style() ? "%k:%M" : "%l:%M"), tick_time);
        strftime(s_day, sizeof(s_day), "%d", tick_time);
        strftime(s_mon, sizeof(s_mon), "%b", tick_time);
        strftime(s_dow, sizeof(s_dow), "%a", tick_time);
        strftime(s_year, sizeof(s_year), "%Y", tick_time);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "update time %s %s %s %s %s", s_time, s_day, s_mon, s_dow, s_year);
    }

    if (time_changed) {
        time_changed();
    }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time(false);
}

void time_init(time_changed_cb callback) {
    time_deinit();

    time_changed = callback;

    TimeUnits units = MINUTE_UNIT;
    if (settings.DisplaySeconds) {
        units = SECOND_UNIT;
    }
    tick_timer_service_subscribe(units, tick_handler);
    update_time(true);
}

void time_deinit() {
    time_changed = NULL;

    tick_timer_service_unsubscribe();
}
