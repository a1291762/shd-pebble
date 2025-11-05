#include <pebble.h>
#include "health.h"
#include "settings.h"
#include "hours.h"

float mSteps;
char stepsChar[12];
float mHours;
char hoursChar[9];
float mMinutes;
char minutesChar[9];

static health_changed_cb health_changed;

#ifdef PBL_HEALTH

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
    int *out_minutes = context;
    if (time_end > time_start) {
        int minutes = (time_end - time_start) / 60;
        struct tm *tick_time = localtime(&time_start);
        char time[8];
        strftime(time, sizeof(time), "%H:%M", tick_time);
        char *act = health_activity_enum(activity);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "activity %s time %s minutes %d", act, time, minutes);
        *out_minutes += minutes;
    }
    return true;
}

static float value_to_percent(int value, int max) {
    if (value < 0) {
        return 0;
    } else if (value < max) {
        return value / (float)max;
    } else {
        return 1.0f;
    }
}

static void health_handler(HealthEventType event, void *context) {
    char *act = health_event_type_enum(event);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "health handler event %s", act);
    if (event == HealthEventSignificantUpdate || event == HealthEventMovementUpdate) {
        time_t now = time(NULL);
        time_t start_of_day = time_start_of_today();

        int steps = health_service_sum_today(HealthMetricStepCount);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "steps %d", steps);
        mSteps = value_to_percent(steps, settings.StepTarget);
        stepsChar[0] = '\0';
        int index = 0;
        if (steps > 1000000) {
            const char *fmt = "%3d,";
            index += snprintf(stepsChar, sizeof(stepsChar), fmt, steps / 1000000);
        }
        if (steps > 1000) {
            const char *fmt = (steps > 1000000) ? "%03d," : "%3d,";
            index += snprintf(&stepsChar[index], sizeof(stepsChar) - index, fmt, steps / 1000);
        }
        const char *fmt = (steps > 1000) ? "%03d" : "%3d";
        snprintf(&stepsChar[index], sizeof(stepsChar) - index, fmt, steps % 1000);

        hours_update();
        int hours = hours_data.hours_active;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hours %d", hours);
        mHours = value_to_percent(hours, settings.HourTarget);
        snprintf(hoursChar, sizeof(hoursChar), "%d", hours);

        int minutes = 0;
        HealthActivityMask activity_mask = HealthActivityWalk | HealthActivityRun | HealthActivityOpenWorkout;
        health_service_activities_iterate(activity_mask, start_of_day, now, HealthIterationDirectionFuture, count_minutes, &minutes);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "minutes %d", minutes);
        mMinutes = value_to_percent(minutes, settings.MinuteTarget);
        snprintf(minutesChar, sizeof(minutesChar), "%d", minutes);
    }

    if (health_changed) {
        health_changed();
    }
}

#endif

void health_init(health_changed_cb callback) {
    health_deinit();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "health_init");

    health_changed = callback;
    mSteps = -1;
    stepsChar[0] = '\0';
    mMinutes = -1;
    minutesChar[0] = '\0';
    mHours = -1;
    hoursChar[0] = '\0';

#ifdef PBL_HEALTH
    if (settings.DisplayHealth) {
        health_service_events_subscribe(health_handler, NULL);
    } else {
        hours_delete();
    }
#endif

    if (health_changed) {
        health_changed();
    }
}

void health_deinit() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "health_deinit");
    health_changed = NULL;
#ifdef PBL_HEALTH
    health_service_events_unsubscribe();
#endif
}
