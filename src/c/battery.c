#include <pebble.h>
#include "settings.h"
#include "battery.h"
#include "geometry.h"

int mBattery = -1;
char batteryChar[5];
static battery_changed_cb battery_changed;

static void battery_handler(BatteryChargeState charge_state) {
    mBattery = charge_state.charge_percent;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "mBattery %d", mBattery);
    if (mBattery < 0) mBattery = 0;
    else if (mBattery > 100) mBattery = 100;

    snprintf(batteryChar, sizeof(batteryChar), "%d", mBattery);
    geometry_battery();

    if (battery_changed) {
        battery_changed();
    }
}

void battery_init(battery_changed_cb callback) {
    battery_deinit();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init");

    battery_changed = callback;
    mBattery = -1;
    batteryChar[0] = '\0';

    if (settings.DisplayBattery) {
        battery_state_service_subscribe(battery_handler);
        // probe for initial battery state
        battery_handler(battery_state_service_peek());
    }
}

void battery_deinit() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "deinit");
    battery_changed = NULL;
    battery_state_service_unsubscribe();
}
