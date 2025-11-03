#include <pebble.h>
#include "health.h"
#include "settings.h"

int mSteps;
char stepsChar[9];
int mMinutes;
char minutesChar[9];

static health_changed_cb health_changed;

static void health_event(HealthEventType event, void *context) {
    if (event == HealthEventSignificantUpdate || event == HealthEventMovementUpdate) {
        mSteps = health_service_sum_today(HealthMetricStepCount);
        snprintf(stepsChar, sizeof(stepsChar), "%d", mSteps);
        mMinutes = health_service_sum_today(HealthMetricActiveSeconds) / 60;
        snprintf(minutesChar, sizeof(minutesChar), "%d", mMinutes);
    }

    if (health_changed) {
        health_changed();
    }
}

void health_init(health_changed_cb callback) {
    health_changed = callback;

    mSteps = -1;
    stepsChar[0] = '\0';
    mMinutes = -1;
    minutesChar[0] = '\0';
    if (settings.DisplayHealth) {
        health_service_events_subscribe(health_event, NULL);
        health_event(HealthEventSignificantUpdate, NULL);
    } else {
        if (health_changed) {
            health_changed();
        }
    }
}

void health_deinit() {
    health_changed = NULL;

    health_service_events_unsubscribe();
}
