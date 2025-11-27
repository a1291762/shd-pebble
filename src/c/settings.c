#include <pebble.h>
#include "settings.h"
#include "keys.h"

struct Settings settings = {
    .DisplaySeconds = false,
    .DisplayBattery = true,
    .DisplayHealth = true,
    .InvertColor = true,
    .PartialInvert = false,
    .StepTarget = 5000,
    .MinuteTarget = 20,
    .HourTarget = 6,
    .AnimateOnLaunch = false,
    .AnimateOnShake = false,
};
static settings_changed_cb settings_changed;

#define ENUM_HELPER(x) case x: return #x;

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox handler");

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

    if ((tuple = dict_find(iter, MESSAGE_KEY_InvertColor))) {
        settings.InvertColor = tuple->value->int32 == 1;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "invert color %d", settings.InvertColor);
    }

    if ((tuple = dict_find(iter, MESSAGE_KEY_PartialInvert))) {
        settings.PartialInvert = tuple->value->int32 == 1;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "partial invert %d", settings.PartialInvert);
    }

    if ((tuple = dict_find(iter, MESSAGE_KEY_StepTarget))) {
        settings.StepTarget = atoi(tuple->value->cstring);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "step target %d", settings.StepTarget);
    }

    if ((tuple = dict_find(iter, MESSAGE_KEY_MinuteTarget))) {
        settings.MinuteTarget = atoi(tuple->value->cstring);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "minute target %d", settings.MinuteTarget);
    }

    if ((tuple = dict_find(iter, MESSAGE_KEY_HourTarget))) {
        settings.HourTarget = atoi(tuple->value->cstring);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hour target %d", settings.HourTarget);
    }

    if ((tuple = dict_find(iter, MESSAGE_KEY_AnimateOnLaunch))) {
        settings.AnimateOnLaunch = tuple->value->int32 == 1;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "animate on launch %d", settings.AnimateOnLaunch);
    }

    if ((tuple = dict_find(iter, MESSAGE_KEY_AnimateOnShake))) {
        settings.AnimateOnShake = tuple->value->int32 == 1;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "animate on shake %d", settings.AnimateOnShake);
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
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init");

    settings_changed = callback;

    if (persist_exists(SETTINGS_KEY) && persist_get_size(SETTINGS_KEY) == sizeof(settings)) {
        // load from config
        persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
    }

    APP_LOG(APP_LOG_LEVEL_DEBUG, "display seconds %d", settings.DisplaySeconds);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "display battery %d", settings.DisplayBattery);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "display health %d", settings.DisplayHealth);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "invert color %d", settings.InvertColor);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "partial invert %d", settings.PartialInvert);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "step target %d", settings.StepTarget);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "minute target %d", settings.MinuteTarget);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "hour target %d", settings.HourTarget);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "animate on launch %d", settings.AnimateOnLaunch);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "animate on shake %d", settings.AnimateOnShake);

    app_message_register_inbox_received(inbox_received_handler);

    Tuplet pairs[] = {
        TupletInteger(MESSAGE_KEY_DisplaySeconds, 1),
        TupletInteger(MESSAGE_KEY_DisplayBattery, 1),
        TupletInteger(MESSAGE_KEY_DisplayHealth, 1),
        TupletInteger(MESSAGE_KEY_InvertColor, 1),
        TupletInteger(MESSAGE_KEY_PartialInvert, 1),
        TupletCString(MESSAGE_KEY_StepTarget, "1000000"),
        TupletCString(MESSAGE_KEY_MinuteTarget, "1440"),
        TupletCString(MESSAGE_KEY_HourTarget, "24"),
        TupletInteger(MESSAGE_KEY_AnimateOnLaunch, 1),
        TupletInteger(MESSAGE_KEY_AnimateOnShake, 1),
    };
    uint32_t size = dict_calc_buffer_size_from_tuplets(pairs, ARRAY_LENGTH(pairs));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox size %lu", size);
    app_message_open(size, size);

    // The settings have always "changed" when we start
    if (settings_changed) {
        settings_changed();
    }
}

void settings_deinit() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "deinit");
    settings_changed = NULL;
    app_message_deregister_callbacks();
}
