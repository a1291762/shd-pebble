#include <pebble.h>
#include "canvas.h"

float mScale;
GRect canvas_bounds;
int canvas_center_x;
int canvas_center_y;
int screen_center_x;
int screen_center_y;

float px(float px) {
    return px * mScale;
}

GRect setArcRect(float topleft) {
    int x = topleft;
    int y = topleft;
    int w = px(400) - topleft - x;
    int h = px(400) - topleft - y;
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "setArcRect %d.%03d -> %d, %d, %d, %d", (int)topleft, (int)(topleft * 1000)%1000, x, y, w, h);
    return GRect(x, y, w, h);
}

void canvas_draw_arc(GContext *ctx, GRect rect, int32_t angle_start, int32_t angle_sweep) {
    angle_start += 90;
    GOvalScaleMode scaleMode = GOvalScaleModeFitCircle;
    int32_t angle_end = angle_start + angle_sweep;
    if (angle_sweep < 0) {
        angle_end = angle_start;
        angle_start = angle_end + angle_sweep;
    }
    graphics_draw_arc(ctx, rect, scaleMode, DEG_TO_TRIGANGLE(angle_start), DEG_TO_TRIGANGLE(angle_end));
}

void canvas_draw_line(GContext *ctx, int x1, int y1, int x2, int y2) {
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "draw line %d, %d - %d, %d", x1, y1, x2, y2);
    graphics_draw_line(ctx, GPoint(x1, y1), GPoint(x2, y2));
}

void canvas_init(GRect window_bounds) {
    int x = window_bounds.origin.x;
    int y = window_bounds.origin.y;
    int w = window_bounds.size.w;
    int h = window_bounds.size.h;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "window bounds are %d %d, %d %d", x, y, w, h);

#if PBL_RECT
    if (PBL_DISPLAY_HEIGHT < 180) {
        // the width is intentionally wider than the actual screen
        w = 400 * 0.4;
    }
    // heigh matches width
    h = w;
    // offset so the image is centered
    x -= (w - window_bounds.size.w) / 2.0;
    y -= (h - window_bounds.size.h) / 2.0;
#endif
    canvas_bounds = GRect(x, y, w, h);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "canvas bounds are %d %d, %d %d", x, y, w, h);

    canvas_center_x = w / 2;
    canvas_center_y = h / 2;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "canvas center is %d %d", canvas_center_x, canvas_center_y);

    screen_center_x = window_bounds.size.w / 2;
    screen_center_y = window_bounds.size.h / 2;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "screen center is %d %d", screen_center_x, screen_center_y);

    if (PBL_DISPLAY_HEIGHT >= 180) {
        mScale = canvas_bounds.size.w / 400.0f; // 1.0 on TicWatch E, 0.9 on TicWatch E3
    } else {
        mScale = 0.4;
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "mScale %d.%03d", (int)mScale, (int)(mScale*1000)%1000);
}
