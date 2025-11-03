#include <pebble.h>
#include "health.h"
#include "settings.h"
#include "hours.h"

int mSteps;
char stepsChar[9];
int mHours;
char hoursChar[9];
int mMinutes;
char minutesChar[9];

static health_changed_cb health_changed;

static bool count_minutes(HealthActivity activity, time_t time_start, time_t time_end, void *context) {
    if (time_end > time_start) {
        int minutes = (time_end - time_start) / 60;
        char *act = "unknown";
        if (activity == HealthActivityWalk) {
            act = "walk";
        } else if (activity == HealthActivityRun) {
            act = "run";
        } else if (activity == HealthActivityOpenWorkout) {
            act = "workout";
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "activity %s minutes %d", act, minutes);
        mMinutes += minutes;
    }
    return true;
}

static void health_handler(HealthEventType event, void *context) {
    if (event == HealthEventSignificantUpdate || event == HealthEventMovementUpdate) {
        time_t now = time(NULL);
        time_t start_of_day = time_start_of_today();

        mSteps = health_service_sum_today(HealthMetricStepCount);
        snprintf(stepsChar, sizeof(stepsChar), "%d", mSteps);

        hours_update();
        mHours = hours_data.hours_active;
        snprintf(hoursChar, sizeof(hoursChar), "%d", mHours);

        mMinutes = 0;
        HealthActivityMask activity_mask = HealthActivityWalk | HealthActivityRun | HealthActivityOpenWorkout;
        health_service_activities_iterate(activity_mask, start_of_day, now, HealthIterationDirectionFuture, count_minutes, NULL);
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
    mHours = -1;
    hoursChar[0] = '\0';

    if (settings.DisplayHealth) {
        health_service_events_subscribe(health_handler, NULL);
    } else {
        hours_delete();
    }

    if (health_changed) {
        health_changed();
    }
}

void health_deinit() {
    health_changed = NULL;

    health_service_events_unsubscribe();
}
