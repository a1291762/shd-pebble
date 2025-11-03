#pragma once

// A structure containing our settings
struct ClaySettings {
  bool DisplayBattery;
  bool DisplaySeconds;
} __attribute__((__packed__));

extern struct ClaySettings settings;

void settings_init();
