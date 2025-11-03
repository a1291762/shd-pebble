#pragma once

struct HoursData {
    time_t day_start;
    uint8_t hours_counted;
    uint8_t hours_active;
} __attribute__((__packed__));

extern struct HoursData hours_data;

void hours_update();
void hours_delete();
