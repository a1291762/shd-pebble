#include <pebble.h>
#include "timer.h"

static AppTimer *s_timer;
static timer_callback s_callback;

static void timer_event(void *context) {
    s_timer = NULL;
    s_callback();
}

void timer_init(timer_callback callback, uint32_t millis) {
    s_callback = callback;
    s_timer = app_timer_register(millis, timer_event, NULL);
}

void timer_deinit() {
    if (s_timer) {
        app_timer_cancel(s_timer);
        s_timer = NULL;
    }
}
