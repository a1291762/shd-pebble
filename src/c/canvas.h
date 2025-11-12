#pragma once

// 400x400 scaled to screen co-ordinates
extern float mScale;

// screen co-ordinates
extern GRect canvas_bounds;
extern int canvas_center_x;
extern int canvas_center_y;
extern int screen_center_x;
extern int screen_center_y;

float px(float px);
GRect setArcRect(float topleft);
void canvas_draw_arc(GContext *ctx, GRect rect, int32_t angle_start, int32_t angle_sweep);
void canvas_draw_line(GContext *ctx, int x1, int y1, int x2, int y2);
void canvas_draw_text(GContext *ctx, char *string, GFont font, GRect box, GTextAlignment alignment);
void canvas_init(GRect window_bounds);
