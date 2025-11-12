#include <pebble.h>
#include "settings.h"
#include "time.h"
#include "geometry.h"

unsigned long now = 0;
char s_time[8];
char s_day[3];
char s_mon[5];
char s_dow[5];
char s_year[5];
static time_changed_cb time_changed;

static void time_update() {
    time_t last_seconds = now / 1000;
    time_t this_seconds = time(NULL);
    time_t last_minute = last_seconds / 60;
    time_t this_minute = this_seconds / 60;
    time_t last_day = last_seconds / 86400;
    time_t this_day = this_seconds / 86400;

    // code ported from Java where time is expressed in milliseconds
    now = this_seconds * 1000;

    if (this_minute != last_minute) {
        struct tm *tick_time = localtime(&this_seconds);
        strftime(s_time, sizeof(s_time), (clock_is_24h_style() ? "%k:%M" : "%l:%M"), tick_time);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "update time %s", s_time);
        if (this_day != last_day) {
            strftime(s_day, sizeof(s_day), "%e", tick_time);
            strftime(s_mon, sizeof(s_mon), "%b", tick_time);
            strftime(s_dow, sizeof(s_dow), "%a", tick_time);
            strftime(s_year, sizeof(s_year), "%Y", tick_time);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "update date %s %s %s %s", s_day, s_mon, s_dow, s_year);
            geometry_date();
        }
    }

    if (time_changed) {
        time_changed();
    }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    time_update();
}

void time_init(time_changed_cb callback) {
    time_deinit();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "time_init");

    time_changed = callback;

    TimeUnits units = MINUTE_UNIT;
    if (settings.DisplaySeconds) {
        units = SECOND_UNIT;
    }
    tick_timer_service_subscribe(units, tick_handler);
    // probe for initial time
    // time_update();
}

void time_deinit() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "time_deinit");
    time_changed = NULL;
    tick_timer_service_unsubscribe();
}
