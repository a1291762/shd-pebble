#include <pebble.h>
#include "settings.h"
#include "battery.h"

int mBattery = -1;
char batteryChar[5];
static battery_changed_cb battery_changed;

static void handle_battery(BatteryChargeState charge_state) {
    mBattery = charge_state.charge_percent;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "mBattery %d", mBattery);

    batteryChar[0] = mBattery == 100 ? '1' : ' ';
    batteryChar[1] = mBattery >= 10 ? (char)(((mBattery % 100) / 10) + (int)'0') : ' ';
    batteryChar[2] = (char)((mBattery % 10) + (int)'0');
    //batteryChar[3] = '%';

    if (battery_changed) {
        battery_changed();
    }
}

void battery_init(battery_changed_cb callback) {
    battery_changed = callback;

    battery_state_service_unsubscribe();
    mBattery = -1;
    batteryChar[0] = '\0';
    if (settings.DisplayBattery) {
        battery_state_service_subscribe(handle_battery);
        handle_battery(battery_state_service_peek());
    }
    if (battery_changed) {
        battery_changed();
    }
}

void battery_deinit() {
    battery_changed = NULL;

    battery_state_service_unsubscribe();
}
