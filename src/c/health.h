#pragma once

extern int mSteps;
extern char stepsChar[];
extern int mHours;
extern char hoursChar[];
extern int mMinutes;
extern char minutesChar[];

typedef void (*health_changed_cb)();
void health_init(health_changed_cb callback);
void health_deinit();
