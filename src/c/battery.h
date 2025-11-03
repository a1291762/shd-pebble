#pragma once

extern int mBattery;
extern char batteryChar[5];

typedef void (*battery_changed_cb)();

void battery_init(battery_changed_cb callback);
void battery_deinit();
