#include <pebble.h>
#include "math.h"
#include "battery.h"
#include "time.h"
#include "health.h"
#include "canvas.h"
#include "palette.h"
#include "geometry.h"
#include "settings.h"
#include "resources.h"
#include "animation.h"

static bool pulsing = false;
static bool showAllTicks;

static void drawCircleBackground(GContext *ctx) {
    graphics_context_set_fill_color(ctx, bgColor);
    graphics_fill_circle(ctx, GPoint(canvas_center_x, canvas_center_y), px(200));
}

static void drawSimpleBackground(GContext *ctx) {
    int gap;
    float angle, sweep;

    // outer ring
    graphics_context_set_stroke_color(ctx, outerRingColor);
    graphics_context_set_stroke_width(ctx, px(14));
    gap = 4; // was 3 but stroke is not square?
    GRect mArcRect = setArcRect(px(34));
    int max_sweep = 120 - gap - gap;
    canvas_draw_arc(ctx, mArcRect, 90+gap, max_sweep);
    canvas_draw_arc(ctx, mArcRect, 90+gap+120, max_sweep);
    canvas_draw_arc(ctx, mArcRect, 90+gap+240, max_sweep);

    if (!pulsing && !animating) {
        // remove the contents of the outer ring segments
        graphics_context_set_stroke_color(ctx, bgColor);
        graphics_context_set_stroke_width(ctx, px(7));
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
    const int skipped_ticks = 1;

    // ticks
    graphics_context_set_stroke_color(ctx, fgColor);
    graphics_context_set_stroke_width(ctx, 1);
    for (int tickIndex = 0; tickIndex < tick_count; tickIndex++) {
        if (mBattery == -1 || animating || showAllTicks || (tickIndex > skipped_ticks && tickIndex < (tick_count - skipped_ticks))) {
            struct Tick *tick = &mTicks[tickIndex];
            canvas_draw_line(ctx, canvas_center_x + tick->innerX, canvas_center_y + tick->innerY,
                canvas_center_x + tick->outerX, canvas_center_y + tick->outerY);
        }
    }

    if (settings.DisplaySeconds) {
        // active tick
        graphics_context_set_stroke_color(ctx, fgColor);
        graphics_context_set_stroke_width(ctx, 2);
        int tickIndex = (int)((now % 60000) / 1000) * (tick_count / 60);
        if (mBattery == -1 || animating || showAllTicks || (tickIndex > skipped_ticks && tickIndex < (tick_count - skipped_ticks))) {
            struct Tick *tick = &mTicks[tickIndex];
            canvas_draw_line(ctx, canvas_center_x + tick->innerX, canvas_center_y + tick->innerY,
                canvas_center_x + tick->outerX, canvas_center_y + tick->outerY);
        }
    }

    // not enough pixels for this?
    // int sweep;
    // if (animating) {
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
void drawAnimations(GContext *ctx) {

    int gap;
    float angle, sweep;
    GRect mArcRect;
    float wibble = px(14);
    float ref = px(60);

    PBL_IF_COLOR_ELSE({
        // inner ring
        graphics_context_set_stroke_color(ctx, innerBandColor);
        graphics_context_set_stroke_width(ctx, px(20));
        mArcRect = setArcRect(ref);
        canvas_draw_arc(ctx, mArcRect, 0, 360);

        // inner ring details
        graphics_context_set_stroke_width(ctx, 1);
        setArcRect(ref - wibble);
        canvas_draw_arc(ctx, mArcRect, 0, 360);
        setArcRect(ref + wibble);
        canvas_draw_arc(ctx, mArcRect, 0, 360);
    }, {});

    // inner ring animation
    int anim_period = 1500;
    int quadrant = (int)(now % (anim_period*4) / anim_period);
    float anim_sweep = 0;
    if (quadrant == 1 || quadrant == 2) {
        anim_sweep = ((now % anim_period) / (float)anim_period) * 360;
    }
    float anim_angle = 270;
    if (quadrant == 2) anim_angle = 270 + anim_sweep;
    if (quadrant == 2) anim_sweep = 359 - anim_sweep;
    if (anim_sweep != 0) {
        // adjust for non-square stokes
        anim_angle += 2;
        anim_sweep -= 2;
        PBL_IF_COLOR_ELSE({
            graphics_context_set_stroke_color(ctx, innerBandBrightColor);
            graphics_context_set_stroke_width(ctx, px(20));
            mArcRect = setArcRect(ref);
            canvas_draw_arc(ctx, mArcRect, anim_angle, anim_sweep);
        }, {});
        graphics_context_set_stroke_color(ctx, innerBandBrightColor);
        graphics_context_set_stroke_width(ctx, 1);
        mArcRect = setArcRect(ref - wibble);
        canvas_draw_arc(ctx, mArcRect, anim_angle, anim_sweep);
        mArcRect = setArcRect(ref + wibble);
        canvas_draw_arc(ctx, mArcRect, anim_angle, anim_sweep);
    }

    // the line
    graphics_context_set_stroke_color(ctx, innerBandLineColor);
    graphics_context_set_stroke_width(ctx, 1);
    mArcRect = setArcRect(ref);
    anim_sweep = 360;
    if (quadrant == 0 || quadrant == 3) {
        anim_sweep = ((now % anim_period) / (float)anim_period) * 360;
    }
    anim_angle = 270;
    if (quadrant == 3) anim_angle = 270 + anim_sweep;
    if (quadrant == 3) anim_sweep = 359 - anim_sweep;
    canvas_draw_arc(ctx, mArcRect, anim_angle, anim_sweep);

    // line highlight
    graphics_context_set_stroke_color(ctx, innerBandLineBrightColor);
    graphics_context_set_stroke_width(ctx, 2);
    int line_period = 1000;
    int line_quadrant = (int)(now % (line_period*6) / line_period);
    angle = -90 + line_quadrant * 360/6.0f;
    sweep = 60;
    if (quadrant == 0 && anim_sweep < 60) {
        sweep = anim_sweep;
    }
    if (quadrant == 3 && anim_sweep < 60) {
        angle += 60;
        sweep = -anim_sweep;
    }
    canvas_draw_arc(ctx, mArcRect, angle, sweep);

    // draw the gaps back in
    graphics_context_set_stroke_color(ctx, bgColor);
    graphics_context_set_stroke_width(ctx, 2);
    int count = 6;
    float outerTickRadius = canvas_center_x - ref + px(10);
    float innerTickRadius = canvas_center_x - ref - px(10);
    float one = CIRCLE / (float)count;

    for (int tickIndex = 0; tickIndex < count; tickIndex++) {
        float tickRot = (one * tickIndex);
        int innerX = math_sin(tickRot) * innerTickRadius;
        int innerY = -math_cos(tickRot) * innerTickRadius;
        int outerX = math_sin(tickRot) * outerTickRadius;
        int outerY = -math_cos(tickRot) * outerTickRadius;
        canvas_draw_line(ctx, canvas_center_x + innerX, canvas_center_y + innerY,
            canvas_center_x + outerX, canvas_center_y + outerY);
    }

    // counter-rotating lines
    mArcRect = setArcRect(px(85));
    PBL_IF_COLOR_ELSE({
        graphics_context_set_stroke_color(ctx, innerBandColor);
        graphics_context_set_stroke_width(ctx, 2);
        canvas_draw_arc(ctx, mArcRect, 0, 360);
    }, {});
    int period = 60000;
    angle = (period - (now % period)) / (float)period * 360;
    sweep = 50;
    graphics_context_set_stroke_color(ctx, innerBandBrightColor);
    graphics_context_set_stroke_width(ctx, 2);
    canvas_draw_arc(ctx, mArcRect, angle, sweep);
    canvas_draw_arc(ctx, mArcRect, angle + 90, sweep);
    canvas_draw_arc(ctx, mArcRect, angle + 180, sweep);
    canvas_draw_arc(ctx, mArcRect, angle + 270, sweep);

    // blobs
    graphics_context_set_stroke_color(ctx, blobColor);
    graphics_context_set_stroke_width(ctx, 4);
    mArcRect = setArcRect(px(98));
    ref = 200;
    angle = ref;
    sweep = 1;
    gap = 6;
    for (int i = 0; i < 6; i++) {
        PBL_IF_BW_ELSE({
            graphics_context_set_stroke_color(ctx, blobBrightColor);
            graphics_context_set_stroke_width(ctx, 6);
            canvas_draw_arc(ctx, mArcRect, angle, sweep);
            graphics_context_set_stroke_color(ctx, blobColor);
            graphics_context_set_stroke_width(ctx, 4);
            canvas_draw_arc(ctx, mArcRect, angle, sweep);
        }, {
            canvas_draw_arc(ctx, mArcRect, angle, sweep);
        });
        angle += sweep + gap;
    }

    // highlighted blob
    graphics_context_set_stroke_color(ctx, blobBrightColor);
    period = 500;
    quadrant = (int)(now % (period*6) / period);
    angle = ref + (5 - quadrant) * (sweep + gap);
    canvas_draw_arc(ctx, mArcRect, angle, sweep);
 
    // expanding circle
    period = 1000;
    // expand, slowing but do not contract
    angle = now % period / (float)period / 1.8f;
    angle = (float)(math_sin(angle * PI));
    mArcRect = setArcRect(px(200) - (int)(angle * px(85)));
    GColor color = expandingColor;
    PBL_IF_COLOR_ELSE({
        // interpolate from black (with a fadeout at the end)
        int r = 0b11;
        int g = 0b01;
        angle = now % period / (float)period / 1.2f;
        angle = (float)(math_sin(angle * PI));
        r = (int)(r * angle);
        g = (int)(g * angle);
        color.r = r;
        color.g = g;
    }, {});
    graphics_context_set_stroke_color(ctx, color);
    graphics_context_set_stroke_width(ctx, px(8));
    canvas_draw_arc(ctx, mArcRect, 0, 360);

    // fast counter-rotating dot
    graphics_context_set_stroke_color(ctx, counterDotColor);
    graphics_context_set_stroke_width(ctx, 1);
    mArcRect = setArcRect(px(48));
    period = 2000;
    angle = (period - (now % period)) / (float)period * 360;
    angle = ((int)angle + 315) % 360; // -45 degree offset
    sweep = 10;
    canvas_draw_arc(ctx, mArcRect, angle, sweep);
}

static void drawForeground(GContext *ctx) {
    graphics_draw_bitmap_in_rect(ctx, logo_bitmap, mLogoBounds);

    // date
    graphics_context_set_text_color(ctx, fgColor);
    canvas_draw_text(ctx, s_day, date_font, mDateBounds[0], GTextAlignmentRight);
    canvas_draw_text(ctx, s_mon, date_font, mDateBounds[1], GTextAlignmentRight);
    canvas_draw_text(ctx, s_dow, date_font, mDateBounds[2], GTextAlignmentLeft);
    canvas_draw_text(ctx, s_year, date_font, mDateBounds[3], GTextAlignmentLeft);

    PBL_IF_COLOR_ELSE({
        // time background
        graphics_context_set_text_color(ctx, timeBgColor);
        canvas_draw_text(ctx, "8", time_font, mTimeBounds[0], GTextAlignmentLeft);
        canvas_draw_text(ctx, "8", time_font, mTimeBounds[1], GTextAlignmentLeft);
        canvas_draw_text(ctx, "8", time_font, mTimeBounds[3], GTextAlignmentLeft);
        canvas_draw_text(ctx, "8", time_font, mTimeBounds[4], GTextAlignmentLeft);
    }, {});

    // time foreground
    graphics_context_set_text_color(ctx, fgColor);
    char timeChar[2] = {0};
    timeChar[0] = s_time[0];
    canvas_draw_text(ctx, timeChar, time_font, mTimeBounds[0], GTextAlignmentLeft);
    timeChar[0] = s_time[1];
    canvas_draw_text(ctx, timeChar, time_font, mTimeBounds[1], GTextAlignmentLeft);
    timeChar[0] = s_time[2];
    canvas_draw_text(ctx, timeChar, time_font, mTimeBounds[2], GTextAlignmentLeft);
    timeChar[0] = s_time[3];
    canvas_draw_text(ctx, timeChar, time_font, mTimeBounds[3], GTextAlignmentLeft);
    timeChar[0] = s_time[4];
    canvas_draw_text(ctx, timeChar, time_font, mTimeBounds[4], GTextAlignmentLeft);

    // debug bounds
    // graphics_context_set_stroke_color(ctx, fgColor);
    // graphics_context_set_stroke_width(ctx, 1);
    // graphics_draw_rect(ctx, mLogoBounds);
    // graphics_draw_rect(ctx, mDateBounds[0]);
    // graphics_draw_rect(ctx, mDateBounds[1]);
    // graphics_draw_rect(ctx, mDateBounds[2]);
    // graphics_draw_rect(ctx, mDateBounds[3]);
    // graphics_draw_rect(ctx, mTimeBounds[0]);
    // graphics_draw_rect(ctx, mTimeBounds[1]);
    // graphics_draw_rect(ctx, mTimeBounds[2]);
    // graphics_draw_rect(ctx, mTimeBounds[3]);
    // graphics_draw_rect(ctx, mTimeBounds[4]);
    // PBL_IF_ROUND_ELSE({
    //     graphics_draw_rect(ctx, mBatteryBounds);
    // }, {});
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

static void drawComplications(GContext *ctx) {
    int gap;
    float angle, sweep, percent;

    if (mBattery != -1) {
        // battery ring
        graphics_context_set_stroke_color(ctx, batteryRingColor);
        graphics_context_set_stroke_width(ctx, 2);
        // if the battery percentage is being written in the ring, leave room for it
        gap = showAllTicks ? 0 : 8;
        GRect mArcRect = setArcRect(px(21));
        angle = 270 - gap;
        sweep = (360 - gap - gap) * (mBattery / 100.0f);
        angle -= sweep;
        canvas_draw_arc(ctx, mArcRect, angle, sweep);

        // battery percent
        PBL_IF_ROUND_ELSE(({
            graphics_context_set_text_color(ctx, fgColor);
            canvas_draw_text(ctx, batteryChar, battery_font, mBatteryBounds, GTextAlignmentCenter);
        }), {});
    }

    if (!pulsing) {
        graphics_context_set_stroke_color(ctx, outerRingColor);
        graphics_context_set_stroke_width(ctx, px(14));
        gap = 4; // was 3 but stroke is not square?
        GRect mArcRect = setArcRect(px(34));

        int max_sweep = 120 - gap - gap;

        if (mHours != -1) {
            // hours draws from the middle out
            angle = 90 + gap + 120 + (max_sweep/2);
            percent = (int)(mHours * max_sweep);
            angle -= percent / 2.0f;
            sweep = percent;
            if (percent) {
                canvas_draw_arc(ctx, mArcRect, angle, sweep);
            }
        }

        if (mMinutes != -1) {
            // minutes fills down
            angle = 90 + gap + max_sweep;
            percent = (int)(mMinutes * max_sweep);
            sweep = -percent;
            if (percent) {
                canvas_draw_arc(ctx, mArcRect, angle, sweep);
            }
        }

        if (mSteps != -1) {
            // steps fills up
            angle = 90 + gap + 240;
            percent = (int)(mSteps * max_sweep);
            sweep = percent;
            if (percent) {
                canvas_draw_arc(ctx, mArcRect, angle, sweep);
            }
        }
    }

//     // complication icons also draw when pulsing
//     if (mHoursIcon != null) {
//         canvas.drawBitmap(mHoursIcon, null, mHoursIconBounds, mIconPaint);
//     }
//     if (mMinutesIcon != null) {
//         canvas.drawBitmap(mMinutesIcon, null, mMinutesIconBounds, mIconPaint);
//     }
//     if (mStepsIcon != null) {
//         canvas.drawBitmap(mStepsIcon, null, mStepsIconBounds, mIconPaint);
//     }

//     // For WearOS 2, show the music icon when music is playing
//     // For WearOS 3, the system shows an icon here when music is playing
//     if (!isWearOS3 && !mMusicCurrent.isEmpty()) {
//         // This icon is identified by mMusicTapBounds
//         canvas.drawText(MUSIC, canvas_center_x, canvas_center_y + px(170), mIconPaint);
//     }
}

void face_layer_update_proc(Layer *layer, GContext *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "draw");

    // background (circle)
    drawCircleBackground(ctx);

    // outer band (hollow)
    drawSimpleBackground(ctx);
    // outer details (including ticks)
    drawComplexBackground(ctx);
    if (animating) {
        // inner details (animating)
        drawAnimations(ctx);
    }

    if (!animating) {
        // date, time, notifications
        drawForeground(ctx);
        drawComplications(ctx);
    }
}

void face_layer_init(Layer *face_layer) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init");

    showAllTicks = PBL_IF_ROUND_ELSE(false, true);

    resources_init();
    geometry_init();
}

void face_layer_deinit() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "deinit");
    resources_deinit();
}
