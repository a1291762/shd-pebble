#include <pebble.h>
#include "geometry.h"
#include "canvas.h"
#include "math.h"
#include "time.h"

struct Tick mTicks[tick_count];
GRect mLogoBounds;
GRect mTimeBounds[5];
static GFont s_date_font;
GRect mDateBounds[4];

void geometry_init(GBitmap *logo_bitmap, GFont time_font, GFont date_font) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "geometry init");

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

    // logo
    GRect bitmap_bounds = gbitmap_get_bounds(logo_bitmap);
    mLogoBounds = GRect(canvas_center_x - bitmap_bounds.size.w / 2 - px(10), px(55),
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

    // date
    s_date_font = date_font;
}

void geometry_date() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "geometry date");
    
    // date
    float x1 = canvas_center_x - px(64);
    float x2 = canvas_center_x + px(45);
    float y1 = px(80);
    float y2 = y1 + px(35);

    GSize daySize = graphics_text_layout_get_content_size(s_day, s_date_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    GSize monSize = graphics_text_layout_get_content_size(s_mon, s_date_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    int leftWidth = daySize.w > monSize.w ? daySize.w : monSize.w;
    x1 -= leftWidth;
    mDateBounds[0] = GRect(x1, y1, leftWidth, daySize.h);
    mDateBounds[1] = GRect(x1, y2, leftWidth, monSize.h);
    GSize dowSize = graphics_text_layout_get_content_size(s_dow, s_date_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    GSize yearSize = graphics_text_layout_get_content_size(s_year, s_date_font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    mDateBounds[2] = GRect(x2, y1, dowSize.w, dowSize.h);
    mDateBounds[3] = GRect(x2, y2, yearSize.w, yearSize.h);
}
