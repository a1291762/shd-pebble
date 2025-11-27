#include <pebble.h>
#include "health.h"
#include "settings.h"
#include "hours.h"
#include "keys.h"
#include "geometry.h"
#include "timer.h"

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

static void update_steps() {
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

    // geometry_health();
    if (health_changed) {
        health_changed();
    }
}

static bool update_hours() {
    time_t this_seconds = time(NULL);
    time_t start_of_day = time_start_of_today();
    bool finished = hours_update(start_of_day, this_seconds);
    int hours = hours_data.hours_active;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "hours %d", hours);
    mHours = value_to_percent(hours, settings.HourTarget);
    snprintf(hoursChar, sizeof(hoursChar), "%d", hours);

    // geometry_health();
    if (health_changed) {
        health_changed();
    }
    return finished;
}

static void update_minutes() {
    time_t this_seconds = time(NULL);
    time_t start_of_day = time_start_of_today();
    int minutes = 0;
    HealthActivityMask activity_mask = HealthActivityWalk | HealthActivityRun | HealthActivityOpenWorkout;
    health_service_activities_iterate(activity_mask, start_of_day, this_seconds, HealthIterationDirectionFuture, count_minutes, &minutes);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "minutes %d", minutes);
    mMinutes = value_to_percent(minutes, settings.MinuteTarget);
    snprintf(minutesChar, sizeof(minutesChar), "%d", minutes);

    // geometry_health();
    if (health_changed) {
        health_changed();
    }
}

static int state;
static void timer_event() {
    switch (state) {
    case 0:
        update_steps();
        timer_init(timer_event, 100);
        state++;
        break;
    case 1:
        update_minutes();
        timer_init(timer_event, 100);
        state++;
        break;
    case 2:
        if (!update_hours()) {
            timer_init(timer_event, 100);
        }
        break;
    }
}

static void health_handler(HealthEventType event, void *context) {
    char *act = health_event_type_enum(event);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "health handler event %s", act);
    if (event == HealthEventSignificantUpdate || event == HealthEventMovementUpdate) {
        // update_steps();
        // update_hours();
        // update_minutes();
        // geometry_health();
        state = 0;
        timer_init(timer_event, 100);
    }
}

#endif

void health_init(health_changed_cb callback) {
    health_deinit();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init");

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
        // probe for initial health status
        // health_handler(HealthEventSignificantUpdate, NULL);
    } else {
        hours_delete();
    }
#endif
}

void health_deinit() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "deinit");
    health_changed = NULL;
#ifdef PBL_HEALTH
    health_service_events_unsubscribe();
#endif
}
