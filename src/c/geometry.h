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

void geometry_init(GBitmap *s_logo_bitmap, GFont s_time_font, GFont s_date_font);
void geometry_date();
void geometry_battery();
void geometry_health();
