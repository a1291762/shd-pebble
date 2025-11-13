#include <pebble.h>
#include "geometry.h"
#include "battery.h"
#include "health.h"
#include "palette.h"
#include "canvas.h"
#include "resources.h"

void ext_layer_update_proc(Layer *layer, GContext *ctx) {
    graphics_context_set_text_color(ctx, extColor);
    canvas_draw_text(ctx, batteryChar, battery_font, mBatteryBounds, GTextAlignmentRight);
    canvas_draw_text(ctx, stepsChar, date_font, mStepsBounds, GTextAlignmentRight);
    canvas_draw_text(ctx, minutesChar, date_font, mMinutesBounds, GTextAlignmentLeft);
    canvas_draw_text(ctx, hoursChar, date_font, mHoursBounds, GTextAlignmentLeft);
}
