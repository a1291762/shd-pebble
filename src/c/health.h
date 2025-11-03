#pragma once

extern float mSteps;
extern char stepsChar[];
extern float mHours;
extern char hoursChar[];
extern float mMinutes;
extern char minutesChar[];

typedef void (*health_changed_cb)();
void health_init(health_changed_cb callback);
void health_deinit();
