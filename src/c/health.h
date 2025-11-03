#pragma once

extern int mSteps;
extern char stepsChar[9];
extern int mHours;
extern char hoursChar[9];
extern int mMinutes;
extern char minutesChar[9];

typedef void (*health_changed_cb)();
void health_init(health_changed_cb callback);
void health_deinit();
