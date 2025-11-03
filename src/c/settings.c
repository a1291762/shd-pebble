#include <pebble.h>
#include "settings.h"

#define SETTINGS_KEY 1

struct Settings settings;
static settings_changed_cb settings_changed;

#define ENUM_HELPER(x) case x: return #x;

static void reset_settings() {
    settings.DisplaySeconds = true;
    settings.DisplayBattery = true;
    settings.DisplayHealth = true;
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox received handler");

    Tuple *tuple;
    if ((tuple = dict_find(iter, MESSAGE_KEY_DisplaySeconds))) {
        settings.DisplaySeconds = tuple->value->int32 == 1;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "display seconds %d", settings.DisplaySeconds);
    }

    if ((tuple = dict_find(iter, MESSAGE_KEY_DisplayBattery))) {
        settings.DisplayBattery = tuple->value->int32 == 1;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "display battery %d", settings.DisplayBattery);
    }

    if ((tuple = dict_find(iter, MESSAGE_KEY_DisplayHealth))) {
        settings.DisplayHealth = tuple->value->int32 == 1;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "display health %d", settings.DisplayHealth);
    }

    // save to config
    persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));

    // update UI
    if (settings_changed) {
        settings_changed();
    }
}

void settings_init(settings_changed_cb callback) {
    settings_deinit();

    settings_changed = callback;

    // load from config
    int read = persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
    // FIXME handle upgrades that expand the settings struct
    if (read == E_DOES_NOT_EXIST || read != sizeof(settings)) {
        reset_settings();
        persist_delete(SETTINGS_KEY);
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "display seconds %d", settings.DisplaySeconds);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "display battery %d", settings.DisplayBattery);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "display health %d", settings.DisplayHealth);

    app_message_register_inbox_received(inbox_received_handler);

    Tuplet pairs[] = {
        TupletInteger(MESSAGE_KEY_DisplaySeconds, 1),
        TupletInteger(MESSAGE_KEY_DisplayBattery, 1),
        TupletInteger(MESSAGE_KEY_DisplayHealth, 1),
    };
    uint32_t size = dict_calc_buffer_size_from_tuplets(pairs, ARRAY_LENGTH(pairs));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox size %u", size);
    app_message_open(size, size);

    if (settings_changed) {
        settings_changed();
    }
}

void settings_deinit() {
    settings_changed = NULL;

    app_message_deregister_callbacks();
}
