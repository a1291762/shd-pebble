#include <pebble.h>
#include "canvas.h"
#include "math.h"
#include "time.h"
#include "palette.h"
#include "battery.h"

static bool showOnlyAnims = false;

void seconds_layer_update_proc(Layer *layer, GContext *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "update seconds layer");
    // ticks
    int ticks = 60;
    float outerTickRadius = canvas_center_x - px(5);
    float innerTickRadius = canvas_center_x - px(15);
    float one = CIRCLE / (float)ticks;

    // active tick
    graphics_context_set_stroke_color(ctx, fgColor);
    graphics_context_set_stroke_width(ctx, 2);
    int tickOffset = (int)((now % 60000) / 1000) * (ticks/60);
    bool showAllTicks = PBL_IF_ROUND_ELSE(false, true);
    if (mBattery == -1 || showOnlyAnims || showAllTicks || (tickOffset > 1 && tickOffset < (ticks - 1))) {
        float tickRot = one * tickOffset;
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickOffset %d", tickOffset);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickRot %d.%03d", (int)tickRot, (int)(tickRot*1000)%1000);
        float innerX = math_sin(tickRot) * innerTickRadius;
        float innerY = -math_cos(tickRot) * innerTickRadius;
        float outerX = math_sin(tickRot) * outerTickRadius;
        float outerY = -math_cos(tickRot) * outerTickRadius;
        canvas_draw_line(ctx, canvas_center_x + innerX, canvas_center_y + innerY, canvas_center_x + outerX, canvas_center_y + outerY);
    }
}
