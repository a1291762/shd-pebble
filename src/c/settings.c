#include <pebble.h>
#include "settings.h"

// defined in each platform's file
void settings_changed();

struct ClaySettings settings;

#define SETTINGS_KEY 1

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *battery = dict_find(iter, MESSAGE_KEY_DisplayBattery);
    if (battery) {
        settings.DisplayBattery = battery->value->int32 == 1;
    }

    Tuple *seconds = dict_find(iter, MESSAGE_KEY_DisplaySeconds);
    if (seconds) {
        settings.DisplaySeconds = seconds->value->int32 == 1;
    }

    // save to config
    persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));

    // update UI
    settings_changed();
}

void settings_init() {
    // default if no config
    settings.DisplayBattery = false;
    settings.DisplaySeconds = false;

    // load from config
    persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));

    app_message_register_inbox_received(inbox_received_handler);
    app_message_open(128, 128);
}
