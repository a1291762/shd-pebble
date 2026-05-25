#pragma once

struct Settings {
    // v1
    bool DisplaySeconds;
    bool DisplayBattery;
    bool DisplayHealth;
    bool InvertColor; // false = Black Background
    bool PartialInvert;
    int StepTarget;
    int MinuteTarget;
    int HourTarget;
    bool AnimateOnLaunch;
    bool AnimateOnShake;
    // v2
    bool UseColor;
} __attribute__((__packed__));

extern struct Settings settings;

typedef void (*settings_changed_cb)();

void settings_init(settings_changed_cb callback);
void settings_deinit();
