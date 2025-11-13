#include <pebble.h>
#include "hours.h"
#include "keys.h"

struct HoursData hours_data = {0, 0, 0};

static void hours_reset() {
    hours_data.day_start = time_start_of_today();
    hours_data.hours_counted = 0;
    hours_data.hours_active = 0;
}

void hours_update(time_t start_of_day, time_t this_seconds) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "update");

    bool needs_persist = false;
    if (hours_data.day_start == 0) {
        int read = persist_read_data(HOURS_KEY, &hours_data, sizeof(hours_data));
        if (read == E_DOES_NOT_EXIST || read != sizeof(hours_data)) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "persistent cache missing");
            hours_reset();
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "persistent cache present");
            // APP_LOG(APP_LOG_LEVEL_DEBUG, "hours_counted %d", hours_data.hours_counted);
            // APP_LOG(APP_LOG_LEVEL_DEBUG, "hours_active %d", hours_data.hours_active);
        }
    }

    if (hours_data.day_start != start_of_day) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "persistent cache out of date");
        hours_reset();
        needs_persist = true;
    }
    
    // for every hour from midnight to the start of the current hour, calculate if enough steps were done
    // 100 steps in the hour counts as active
    time_t start_of_hour = this_seconds / 3600 * 3600;
    int current_hour = (start_of_hour - start_of_day) / 3600;
    // we can only use minute data for whole hours so don't actually count the current hour
    for (int i = hours_data.hours_counted; i < current_hour; i++) {
        time_t start = start_of_day + (i * 3600);
        time_t end = start + 3600;

        int steps_this_hour = 0;
        HealthMinuteData minute_data[60];
        int records = health_service_get_minute_history(minute_data, 60, &start, &end);
        for (int j = 0; j < records; j++) {
            if (minute_data[j].is_invalid) {
                continue;
            }
            steps_this_hour += minute_data[j].steps;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hour %d steps %d", i, steps_this_hour);
        if (steps_this_hour >= 100) {
            hours_data.hours_active++;
        }
        // do not count this hour again
        hours_data.hours_counted++;
        needs_persist = true;
    }

    // If we get to 100 steps in this hour, stop doing this
    if (hours_data.hours_counted == current_hour) {
        int steps_this_hour = health_service_sum(HealthMetricStepCount, start_of_hour, this_seconds);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hour %d steps %d", current_hour, steps_this_hour);
        if (steps_this_hour >= 100) {
            hours_data.hours_active++;
            // do not count this hour again
            hours_data.hours_counted++;
            needs_persist = true;
        }
    }

    if (needs_persist) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "write persistent cache");
        persist_write_data(HOURS_KEY, &hours_data, sizeof(hours_data));
    }
}

void hours_delete() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "delete");
    hours_reset();
    hours_data.day_start = 0;
    persist_delete(HOURS_KEY);
}
