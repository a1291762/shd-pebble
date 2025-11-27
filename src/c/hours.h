#pragma once

struct HoursData {
    time_t day_start;
    uint8_t hours_counted;
    uint8_t hours_active;
} __attribute__((__packed__));

extern struct HoursData hours_data;

bool hours_update(time_t start_of_day, time_t this_seconds);
void hours_delete();
