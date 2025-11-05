#include <pebble.h>
#include "hours.h"

#define HOURS_KEY 2

struct HoursData hours_data = {0, 0, 0};

static void hours_reset() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "hours_reset");
    hours_data.day_start = time_start_of_today();
    hours_data.hours_counted = 0;
    hours_data.hours_active = 0;
}

void hours_update() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "hours_update");
    time_t now = time(NULL);
    time_t start_of_hour = now / 3600 * 3600;
    time_t start_of_day = time_start_of_today();

    int read = persist_read_data(HOURS_KEY, &hours_data, sizeof(hours_data));
    if (read == E_DOES_NOT_EXIST || read != sizeof(hours_data)) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hours persistent cache missing");
        hours_reset();
    } else if (hours_data.day_start != start_of_day) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hours persistent cache out of date");
        hours_reset();
    } else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hours persistent cache present");
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hours_counted %d", hours_data.hours_counted);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hours_active %d", hours_data.hours_active);
    }

    // for every hour from midnight to the start of the current hour, calculate if enough steps were done
    // 100 steps in the hour counts as active
    for (int i = hours_data.hours_counted; i < 23; i++) {
        time_t start = start_of_day + (i * 3600);
        time_t end = start + 3600;
        if (end > start_of_hour) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "hour %d is the current hour", i);
            break;
        }

        int steps_this_hour = 0;
        HealthMinuteData minute_data[60];
        int records = health_service_get_minute_history(minute_data, 60, &start, &end);
        for (int j = 0; j < records; j++) {
            if (minute_data[j].is_invalid) {
                continue;
            }
            steps_this_hour += minute_data[j].steps;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "persisting hour %d steps %d", i, steps_this_hour);
        if (steps_this_hour >= 100) {
            hours_data.hours_active++;
        }
        // do not count this hour again
        hours_data.hours_counted++;
    }

    persist_write_data(HOURS_KEY, &hours_data, sizeof(hours_data));

    int steps_this_hour = health_service_sum(HealthMetricStepCount, start_of_hour, now);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "current hour steps %d", steps_this_hour);
    if (steps_this_hour >= 100) {
        hours_data.hours_active++;
    }
}

void hours_delete() {
    hours_reset();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "hours_delete");
    hours_data.day_start = 0;
    persist_delete(HOURS_KEY);
}
