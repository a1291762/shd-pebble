#include <pebble.h>
#include "settings.h"

void main_window_load(Window *window);
void main_window_unload(Window *window);

static Window *s_main_window;

static void init() {
    settings_init();

    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    const bool animated = true;
    window_stack_push(s_main_window, animated);
}

static void deinit() {
    window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
