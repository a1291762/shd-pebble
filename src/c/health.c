#include <pebble.h>
#include "health.h"
#include "settings.h"
#include "hours.h"

int mSteps;
char stepsChar[12];
int mHours;
char hoursChar[9];
int mMinutes;
char minutesChar[9];

static health_changed_cb health_changed;

#define ENUM_HELPER(x) case x: return #x;

static char *health_activity_enum(HealthActivity activity) {
    switch (activity) {
        ENUM_HELPER(HealthActivityWalk)
        ENUM_HELPER(HealthActivityRun)
        ENUM_HELPER(HealthActivityOpenWorkout)
        ENUM_HELPER(HealthActivityNone)
        ENUM_HELPER(HealthActivitySleep)
        ENUM_HELPER(HealthActivityRestfulSleep)
    }
    return "unknown";
}

static char *health_event_type_enum(HealthEventType event) {
    switch (event) {
        ENUM_HELPER(HealthEventSignificantUpdate)
        ENUM_HELPER(HealthEventMovementUpdate)
        ENUM_HELPER(HealthEventSleepUpdate)
        ENUM_HELPER(HealthEventMetricAlert)
        ENUM_HELPER(HealthEventHeartRateUpdate)
    }
    return "unknown";
}

static bool count_minutes(HealthActivity activity, time_t time_start, time_t time_end, void *context) {
    if (time_end > time_start) {
        int minutes = (time_end - time_start) / 60;
        struct tm *tick_time = localtime(&time_start);
        char time[8];
        strftime(time, sizeof(time), "%H:%M", tick_time);
        char *act = health_activity_enum(activity);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "activity %s time %s minutes %d", act, time, minutes);
        mMinutes += minutes;
    }
    return true;
}

static void health_handler(HealthEventType event, void *context) {
    char *act = health_event_type_enum(event);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "health handler event %s", act);
    if (event == HealthEventSignificantUpdate || event == HealthEventMovementUpdate) {
        time_t now = time(NULL);
        time_t start_of_day = time_start_of_today();

        mSteps = health_service_sum_today(HealthMetricStepCount);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "mSteps %d", mSteps);
        stepsChar[0] = '\0';
        int index = 0;
        if (mSteps > 1000000) {
            const char *fmt = "%3d,";
            index += snprintf(stepsChar, sizeof(stepsChar), fmt, mSteps / 1000000);
        }
        if (mSteps > 1000) {
            const char *fmt = (mSteps > 1000000) ? "%03d," : "%3d,";
            index += snprintf(&stepsChar[index], sizeof(stepsChar) - index, fmt, mSteps / 1000);
        }
        const char *fmt = (mSteps > 1000) ? "%03d," : "%3d,";
        snprintf(&stepsChar[index], sizeof(stepsChar) - index, fmt, mSteps % 1000);

        hours_update();
        mHours = hours_data.hours_active;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "mHours %d", mHours);
        snprintf(hoursChar, sizeof(hoursChar), "%d", mHours);

        mMinutes = 0;
        HealthActivityMask activity_mask = HealthActivityWalk | HealthActivityRun | HealthActivityOpenWorkout;
        health_service_activities_iterate(activity_mask, start_of_day, now, HealthIterationDirectionFuture, count_minutes, NULL);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "mMinutes %d", mMinutes);
        snprintf(minutesChar, sizeof(minutesChar), "%d", mMinutes);
    }

    if (health_changed) {
        health_changed();
    }
}

void health_init(health_changed_cb callback) {
    health_deinit();

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
