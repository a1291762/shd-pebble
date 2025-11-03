#pragma once

struct Settings {
  bool DisplaySeconds;
  bool DisplayBattery;
  bool DisplayHealth;
} __attribute__((__packed__));

extern struct Settings settings;

typedef void (*settings_changed_cb)();

void settings_init(settings_changed_cb callback);
void settings_deinit();
