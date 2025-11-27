#include <pebble.h>
#include "geometry.h"
#include "canvas.h"
#include "math.h"
#include "time.h"
#include "resources.h"

struct Tick mTicks[tick_count];
GRect mLogoBounds;
GRect mTimeBounds[5];
GRect mDateBounds[4];
GRect mBatteryBounds;
GRect mStepsBounds;
GRect mMinutesBounds;
GRect mHoursBounds;

void geometry_init() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init");

    // ticks
    float outerTickRadius = canvas_center_x - px(5);
    float innerTickRadius = canvas_center_x - px(15);
    float one = CIRCLE / (float)tick_count;

    for (int tickIndex = 0; tickIndex < tick_count; tickIndex++) {
        float tickRot = (one * tickIndex);
        mTicks[tickIndex].innerX = math_sin(tickRot) * innerTickRadius;
        mTicks[tickIndex].innerY = -math_cos(tickRot) * innerTickRadius;
        mTicks[tickIndex].outerX = math_sin(tickRot) * outerTickRadius;
        mTicks[tickIndex].outerY = -math_cos(tickRot) * outerTickRadius;
    }

    // logo - shifted slightly left
    GRect bitmap_bounds = gbitmap_get_bounds(logo_bitmap);
    mLogoBounds = GRect(canvas_center_x - bitmap_bounds.size.w / 2 - 3, px(65),
        bitmap_bounds.size.w, bitmap_bounds.size.h);

    // time
    GSize textSize = graphics_text_layout_get_content_size("8", time_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    int digitWidth = textSize.w;
    int digitHeight = textSize.h;
    float x1 = canvas_center_x - 2.1 * digitWidth;
    float x2 = x1 + 3.5 * digitWidth;
    float y = canvas_center_y - digitWidth * 0.7;
    mTimeBounds[0] = GRect(x1, y, digitWidth, digitHeight);
    mTimeBounds[1] = GRect(x1 + digitWidth, y, digitWidth, digitHeight);
    mTimeBounds[2] = GRect(x1 + 2 * digitWidth, y, digitWidth, digitHeight);
    mTimeBounds[3] = GRect(x2 - digitWidth, y, digitWidth, digitHeight);
    mTimeBounds[4] = GRect(x2, y, digitWidth, digitHeight);

    geometry_battery();
    geometry_health();
}

void geometry_date() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "date");
    
    // date
    float x1 = canvas_center_x - px(64);
    float x2 = canvas_center_x + px(50);
#if PBL_DISPLAY_HEIGHT >= 180
    float y1 = px(70);
    float y2 = y1 + px(40);
#else
    float y1 = px(80);
    float y2 = y1 + px(35);
#endif

    GSize daySize = graphics_text_layout_get_content_size(s_day, date_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    GSize monSize = graphics_text_layout_get_content_size(s_mon, date_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    int leftWidth = daySize.w > monSize.w ? daySize.w : monSize.w;
    x1 -= leftWidth;
    mDateBounds[0] = GRect(x1, y1, leftWidth, daySize.h);
    mDateBounds[1] = GRect(x1, y2, leftWidth, monSize.h);
    GSize dowSize = graphics_text_layout_get_content_size(s_dow, date_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    GSize yearSize = graphics_text_layout_get_content_size(s_year, date_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    mDateBounds[2] = GRect(x2, y1, dowSize.w, dowSize.h);
    mDateBounds[3] = GRect(x2, y2, yearSize.w, yearSize.h);
}

void geometry_battery() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "battery");

    GSize textSize = graphics_text_layout_get_content_size("100", battery_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    // why is there extra space above the text???
    const int vfudge = textSize.h * 0.3;
    PBL_IF_ROUND_ELSE({
        mBatteryBounds = GRect(canvas_center_x - textSize.w/2, -vfudge, textSize.w, textSize.h);
    }, {
        // don't hit the edge of the screen
        const int hfudge = px(8);
        int w = screen_center_x * 2;
        mBatteryBounds = GRect(0, -vfudge, w - hfudge, 100);
    });
}

void geometry_health() {
    PBL_IF_RECT_ELSE({
        APP_LOG(APP_LOG_LEVEL_DEBUG, "health");
        
        GSize textSize = graphics_text_layout_get_content_size("100", date_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
        // why is there extra space above the text???
        const int vfudge = textSize.h * 0.2;
        // don't hit the edge of the screen
        const int hfudge = px(8);
        int w = screen_center_x * 2;
        int y = screen_center_y*2 - textSize.h - vfudge;
        mStepsBounds = GRect(0, y, w - hfudge, 100);
        mMinutesBounds = GRect(hfudge, y, w, 100);
        mHoursBounds = GRect(hfudge, -vfudge, w, 100);
    }, {});
}
