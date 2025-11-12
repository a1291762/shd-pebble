#include <pebble.h>
#include "geometry.h"
#include "battery.h"
#include "health.h"
#include "palette.h"
#include "canvas.h"

static GFont s_date_font;

void ext_layer_update_proc(Layer *layer, GContext *ctx) {
    graphics_context_set_text_color(ctx, extColor);
    canvas_draw_text(ctx, batteryChar, s_date_font, mBatteryBounds, GTextAlignmentRight);
    canvas_draw_text(ctx, stepsChar, s_date_font, mStepsBounds, GTextAlignmentRight);
    canvas_draw_text(ctx, minutesChar, s_date_font, mMinutesBounds, GTextAlignmentLeft);
    canvas_draw_text(ctx, hoursChar, s_date_font, mHoursBounds, GTextAlignmentLeft);
}

void ext_layer_init() {
    s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
}
