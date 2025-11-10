#include <pebble.h>
#include "math.h"
#include "battery.h"
#include "time.h"
#include "health.h"
#include "canvas.h"
#include "palette.h"

static GBitmap *s_logo_bitmap;
static bool pulsing = false;
static bool mAmbient = false;
static bool showOnlyAnims = false;

static void drawCircleBackground(GContext *ctx) {
    graphics_context_set_fill_color(ctx, bgColor);
    graphics_fill_circle(ctx, GPoint(canvas_center_x, canvas_center_y), px(200));
}

static void drawSimpleBackground(GContext *ctx) {
    int gap;
    float angle, sweep;

    // outer ring
    graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(GColorOrange, fgColor));
    graphics_context_set_stroke_width(ctx, px(14));
    gap = 4; // was 3 but stroke is not square?
    GRect mArcRect = setArcRect(px(34));
    int max_sweep = 120 - gap - gap;
    canvas_draw_arc(ctx, mArcRect, 90+gap, max_sweep);
    canvas_draw_arc(ctx, mArcRect, 90+gap+120, max_sweep);
    canvas_draw_arc(ctx, mArcRect, 90+gap+240, max_sweep);

    if (!pulsing && !showOnlyAnims) {
        // remove the contents of the outer ring segments
        graphics_context_set_stroke_color(ctx, bgColor);
        graphics_context_set_stroke_width(ctx, px(8));
        sweep = 120 - gap - gap - 2;
        angle = 90 + gap + 1;
        canvas_draw_arc(ctx, mArcRect, angle, sweep);

        angle = 90 + gap + 120 + 1;
        canvas_draw_arc(ctx, mArcRect, angle, sweep);

        angle = 90 + gap + 240 + 1;
        canvas_draw_arc(ctx, mArcRect, angle, sweep);
    }
}

static void drawComplexBackground(GContext *ctx) {
    // ticks
    int ticks = 60;
    graphics_context_set_stroke_color(ctx, fgColor);
    graphics_context_set_stroke_width(ctx, 1); //px(2)
    float outerTickRadius = canvas_center_x - px(5);
    float innerTickRadius = canvas_center_x - px(15);
    float one = CIRCLE / (float)ticks;

    for (int tickIndex = 0; tickIndex < ticks; tickIndex++) {
        float tickRot = (one * tickIndex);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickIndex %d", tickIndex);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickRot %d.%03d", (int)tickRot, (int)(tickRot*1000)%1000);
        float tick_innerX = math_sin(tickRot) * innerTickRadius;
        float tick_innerY = -math_cos(tickRot) * innerTickRadius;
        float tick_outerX = math_sin(tickRot) * outerTickRadius;
        float tick_outerY = -math_cos(tickRot) * outerTickRadius;
        bool showAllTicks = PBL_IF_ROUND_ELSE(false, true);
        if (mBattery == -1 || showOnlyAnims || showAllTicks || (tickIndex > 1 && tickIndex < (ticks - 1))) {
            canvas_draw_line(ctx, canvas_center_x + tick_innerX, canvas_center_y + tick_innerY,
                    canvas_center_x + tick_outerX, canvas_center_y + tick_outerY);
        }
    }

    // not enough pixels for this?
    // int sweep;
    // if (showOnlyAnims) {
    //     // left half circle
    //     graphics_context_set_stroke_color(ctx, fgColor);
    //     graphics_context_set_stroke_width(ctx, 1);
    //     GRect mArcRect = setArcRect(px(21));
    //     canvas_draw_arc(ctx, mArcRect, 90, 180);
    // } else {
    //     // outer ring details
    //     graphics_context_set_stroke_color(ctx, fgColor);
    //     graphics_context_set_stroke_width(ctx, 1);
    //     GRect mArcRect = setArcRect(px(22));
    //     canvas_draw_arc(ctx, mArcRect, 0, 360);
    // }

    // // outer ring details 2
    // graphics_context_set_stroke_color(ctx, fgColor);
    // graphics_context_set_stroke_width(ctx, 1);
    // GRect mArcRect = setArcRect(px(24));
    // sweep = 16;
    // canvas_draw_arc(ctx, mArcRect, 90 + 60 - sweep / 2.0f, sweep);
    // canvas_draw_arc(ctx, mArcRect, 90 + 120 + 60 - sweep / 2.0f, sweep);
    // canvas_draw_arc(ctx, mArcRect, 90 + 240 + 60 - sweep / 2.0f, sweep);
    // sweep = 4;
    // canvas_draw_arc(ctx, mArcRect, 90 + 2.5f, sweep);
    // canvas_draw_arc(ctx, mArcRect, 90 - 2.5f - sweep, sweep);
    // canvas_draw_arc(ctx, mArcRect, 90 + 120 + 2.5f, sweep);
    // canvas_draw_arc(ctx, mArcRect, 90 + 120 - 2.5f - sweep, sweep);
    // canvas_draw_arc(ctx, mArcRect, 90 + 240 + 2.5f, sweep);
    // canvas_draw_arc(ctx, mArcRect, 90 + 240 - 2.5f - sweep, sweep);
}

// implements gratuitous animations as per
// https://www.youtube.com/watch?v=AQdDVrHOKB8
// void drawAnimations(GContext *ctx) {

//     long now = this.now;
//     if (targetFps == 1) {
//         // to prevent wonky animation, pin the time to the start
//         // of the current minute
//         now = (this.now / 60000 * 60000);
//     }

//     int gap;
//     float wibble;
//     float angle, sweep;

//     // inner ring
//     float ref = px(70);
//     setArcRect(ref);
//     canvas.drawArc(mArcRect, 0, 360, false, mInnerBandPaint);

//     // inner ring details
//     wibble = px(14);
//     setArcRect(ref - wibble);
//     canvas.drawArc(mArcRect, 0, 360, false, mInnerBandDetailPaint);

//     setArcRect(ref + wibble);
//     canvas.drawArc(mArcRect, 0, 360, false, mInnerBandDetailPaint);

//     // inner ring animation
//     int anim_period = 1500;
//     int quadrant = (int)(now % (anim_period*4) / anim_period);
//     float anim_sweep = 0;
//     if (quadrant == 1 || quadrant == 2) {
//         anim_sweep = ((now % anim_period) / (float)anim_period) * 360;
//     }
//     float anim_angle = 270;
//     if (quadrant == 2) anim_angle = 270 + anim_sweep;
//     if (quadrant == 2) anim_sweep = 359 - anim_sweep;
//     setArcRect(ref);
//     canvas.drawArc(mArcRect, anim_angle, anim_sweep, false, mInnerBandBrightPaint);
//     setArcRect(ref - wibble);
//     canvas.drawArc(mArcRect, anim_angle, anim_sweep, false, mInnerBandDetailBrightPaint);
//     setArcRect(ref + wibble);
//     canvas.drawArc(mArcRect, anim_angle, anim_sweep, false, mInnerBandDetailBrightPaint);

//     // the line
//     setArcRect(ref);
//     anim_sweep = 360;
//     if (quadrant == 0 || quadrant == 3) {
//         anim_sweep = ((now % anim_period) / (float)anim_period) * 360;
//     }
//     anim_angle = 270;
//     if (quadrant == 3) anim_angle = 270 + anim_sweep;
//     if (quadrant == 3) anim_sweep = 359 - anim_sweep;
//     canvas.drawArc(mArcRect, anim_angle, anim_sweep, false, mInnerBandLinePaint);

//     // line highlight
//     int line_period = 1000;
//     int line_quadrant = (int)(now % (line_period*6) / line_period);
//     angle = -90 + line_quadrant * 360/6f;
//     sweep = 60;
//     if (quadrant == 0 && anim_sweep < 60) {
//         sweep = anim_sweep;
//     }
//     if (quadrant == 3 && anim_sweep < 60) {
//         angle += 60;
//         sweep = -anim_sweep;
//     }
//     canvas.drawArc(mArcRect, angle, sweep, false, mInnerBandLineBrightPaint);

//     // draw the gaps back in
//     gap = 1;
//     canvas.drawArc(mArcRect, 90-gap, gap+gap, false, mInnerBandBlackPaint);
//     canvas.drawArc(mArcRect, 90-gap+120, gap+gap, false, mInnerBandBlackPaint);
//     canvas.drawArc(mArcRect, 90-gap+240, gap+gap, false, mInnerBandBlackPaint);
//     canvas.drawArc(mArcRect, 90-gap+60, gap+gap, false, mInnerBandBlackPaint);
//     canvas.drawArc(mArcRect, 90-gap+180, gap+gap, false, mInnerBandBlackPaint);
//     canvas.drawArc(mArcRect, 90-gap+300, gap+gap, false, mInnerBandBlackPaint);

//     // counter-rotating lines
//     setArcRect(px(95));
//     canvas.drawArc(mArcRect, 0, 360, false, mCounterRotatingLinePaint);
//     int period = 60000;
//     angle = (period - (now % period)) / (float)period * 360;
//     sweep = 50;
//     canvas.drawArc(mArcRect, angle, sweep, false, mCounterRotatingLineBrightPaint);
//     canvas.drawArc(mArcRect, angle + 90, sweep, false, mCounterRotatingLineBrightPaint);
//     canvas.drawArc(mArcRect, angle + 180, sweep, false, mCounterRotatingLineBrightPaint);
//     canvas.drawArc(mArcRect, angle + 270, sweep, false, mCounterRotatingLineBrightPaint);

//     // blobs
//     setArcRect(px(108));
//     ref = 200;
//     angle = ref;
//     sweep = 4;
//     gap = 2;
//     canvas.drawArc(mArcRect, angle, sweep, false, mBlobPaint);
//     angle += sweep + gap;
//     canvas.drawArc(mArcRect, angle, sweep, false, mBlobPaint);
//     angle += sweep + gap;
//     canvas.drawArc(mArcRect, angle, sweep, false, mBlobPaint);
//     angle += sweep + gap;
//     canvas.drawArc(mArcRect, angle, sweep, false, mBlobPaint);
//     angle += sweep + gap;
//     canvas.drawArc(mArcRect, angle, sweep, false, mBlobPaint);
//     angle += sweep + gap;
//     canvas.drawArc(mArcRect, angle, sweep, false, mBlobPaint);

//     // highlighted blob
//     period = 500;
//     quadrant = (int)(now % (period*6) / period);
//     angle = ref + (5 - quadrant) * (sweep + gap);
//     canvas.drawArc(mArcRect, angle, sweep, false, mBlobBrightPaint);

//     // expanding circle
//     period = 1000;
//     // expand, slowing but do not contract
//     angle = now % period / (float)period / 1.8f;
//     if (systemAmbientTime != 0) {
//         angle = (1/1.8f) - angle;
//     }
//     angle = (float)(Math.sin(angle * Math.PI));
//     setArcRect(px(200) - (int)(angle * px(82)));
//     // interpolate from black (with a fadeout at the end)
//     int r = (currentPalette.explodingCircle >> 16) & 0xff;
//     int g = (currentPalette.explodingCircle >> 8) & 0xff;
//     int b = 0;
//     angle = now % period / (float)period / 1.2f;
//     if (systemAmbientTime != 0) {
//         angle = (1/1.2f) - angle;
//     }
//     angle = (float)(Math.sin(angle * Math.PI));
//     r = (int)(r * angle);
//     g = (int)(g * angle);
//     mExplodingCirclePaint.setColor(Color.rgb(r, g, b));
//     canvas.drawArc(mArcRect, 0, 360, false, mExplodingCirclePaint);

//     if (showOnlyAnims) {
//         // fast counter-rotating dot
//         setArcRect(px(50));
//         period = 2000;
//         angle = (period - (now % period)) / (float)period * 360;
//         angle = (angle + 315) % 360; // -45 degree offset
//         sweep = 10;
//         canvas.drawArc(mArcRect, angle, sweep, false, mCounterRotatingLineBrightPaint);
//     }
// }

static void drawForeground(GContext *ctx) {
    GRect bounds = gbitmap_get_bounds(s_logo_bitmap);
    graphics_draw_bitmap_in_rect(ctx, s_logo_bitmap, GRect(canvas_center_x - bounds.size.w / 2 - px(10), px(55), bounds.size.w, bounds.size.h));
}

// static void drawOverlapping(GContext *ctx) {
//     // outer ring details 3
//     graphics_context_set_stroke_color(ctx, fgColor);
//     graphics_context_set_stroke_width(ctx, 1);
//     float innerTickRadius = canvas_center_x - px(24);
//     float outerTickRadius = canvas_center_x - px(24) + px(8);
//     float one = CIRCLE / (float)360;
//     float angles[6] = {
//         180 - 3,
//         180 + 3,
//         180 + 120 - 3,
//         180 + 120 + 3,
//         180 + 240 - 3,
//         180 + 240 + 3,
//     };
//     for (int i = 0; i < 6; i++) {
//         float angle = angles[i];
//         float tickRot = one * angle;
//         float tick_innerX = math_sin(tickRot) * innerTickRadius;
//         float tick_innerY = -math_cos(tickRot) * innerTickRadius;
//         float tick_outerX = math_sin(tickRot) * outerTickRadius;
//         float tick_outerY = -math_cos(tickRot) * outerTickRadius;
//         canvas_draw_line(ctx, canvas_center_x + tick_innerX, canvas_center_y + tick_innerY,
//                 canvas_center_x + tick_outerX, canvas_center_y + tick_outerY);
//     }
// }

void background_layer_update_proc(Layer *layer, GContext *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "update background layer");
    // background (circle)
    drawCircleBackground(ctx);

    // outer band (hollow)
    drawSimpleBackground(ctx);
    if (!mAmbient) {
        // outer details (including ticks)
        drawComplexBackground(ctx);
        // inner details (animating)
        // drawAnimations(ctx);
    }

    if (!showOnlyAnims) {
        // date, time, notifications
        drawForeground(ctx);
    }

    // if (!mAmbient) {
    //     // outer details
    //     drawOverlapping(ctx);
    // }
}

void background_settings_changed() {
    GColor *pal = gbitmap_get_palette(s_logo_bitmap);
    pal[0] = bgColor;
    pal[1] = fgColor;
    gbitmap_set_palette(s_logo_bitmap, pal, false);
}

void background_init() {
    s_logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLLOW);
}

void background_deinit() {
    gbitmap_destroy(s_logo_bitmap);
}
