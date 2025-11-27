#pragma once

struct Tick {
    float innerX;
    float innerY;
    float outerX;
    float outerY;
};
#define tick_count 60
extern struct Tick mTicks[];
extern GRect mLogoBounds;
extern GRect mTimeBounds[];
extern GRect mDateBounds[];
extern GRect mBatteryBounds;
extern GRect mStepsBounds;
extern GRect mMinutesBounds;
extern GRect mHoursBounds;

void geometry_init();
void geometry_date();
