#include <pebble.h>
#include "math.h"
#include "battery.h"
#include "time.h"
#include "health.h"
#include "canvas.h"
#include "palette.h"

static GBitmap *s_logo_bitmap;
static GFont s_time_font;
static char *smallFont = FONT_KEY_GOTHIC_18_BOLD;

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
    graphics_context_set_stroke_color(ctx, outerRingColor);
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

    // date
    GFont date_font = fonts_get_system_font(smallFont);
    float x1 = px(55);
    float y = px(70);
    float y2 = y + px(30);

    graphics_context_set_text_color(ctx, fgColor);
    if (s_day[0] == '0') {
        s_day[0] = ' ';
    }
    canvas_draw_text(ctx, s_day, date_font, GPoint(x1, y), GTextAlignmentRight);
    canvas_draw_text(ctx, s_mon, date_font, GPoint(x1, y2), GTextAlignmentRight);

    x1 = canvas_center_x + px(45);
    canvas_draw_text(ctx, s_dow, date_font, GPoint(x1, y), GTextAlignmentLeft);
    canvas_draw_text(ctx, s_year, date_font, GPoint(x1, y2), GTextAlignmentLeft);

    GTextOverflowMode overflow_mode = GTextOverflowModeWordWrap;
    GTextAlignment alignment = GTextAlignmentLeft;
    GSize textSize = graphics_text_layout_get_content_size("8", s_time_font, GRect(0, 0, 100, 100), overflow_mode, alignment);
    float digitWidth = textSize.w;
    x1 = canvas_center_x - 2.1*digitWidth;
    float x2 = x1 + 3.5*digitWidth;
    y = canvas_center_y - digitWidth * 0.7;
    float t1 = x1;
    float t2 = x1 + digitWidth;
    float t3 = x2 - digitWidth;
    float t4 = x2;
    float t5 = x1 + 2*digitWidth;

    // // time background
    // graphics_context_set_text_color(ctx, fgColor);
    // graphics_draw_text(ctx, "8", s_time_font, GRect(t1, y, digitWidth*2, digitWidth), overflow_mode, alignment, NULL);
    // graphics_draw_text(ctx, "8", s_time_font, GRect(t2, y, digitWidth*2, digitWidth), overflow_mode, alignment, NULL);
    // graphics_draw_text(ctx, "8", s_time_font, GRect(t3, y, digitWidth*2, digitWidth), overflow_mode, alignment, NULL);
    // graphics_draw_text(ctx, "8", s_time_font, GRect(t4, y, digitWidth*2, digitWidth), overflow_mode, alignment, NULL);

    // time foreground
    graphics_context_set_text_color(ctx, fgColor);
    char timeChar[2] = {0};
    if (s_time[0] != ' ') {
        timeChar[0] = s_time[0];
        graphics_draw_text(ctx, timeChar, s_time_font, GRect(t1, y, digitWidth*2, digitWidth), overflow_mode, alignment, NULL);
    }
    timeChar[0] = s_time[1];
    graphics_draw_text(ctx, timeChar, s_time_font, GRect(t2, y, digitWidth*2, digitWidth), overflow_mode, alignment, NULL);
    timeChar[0] = s_time[3];
    graphics_draw_text(ctx, timeChar, s_time_font, GRect(t3, y, digitWidth*2, digitWidth), overflow_mode, alignment, NULL);
    timeChar[0] = s_time[4];
    graphics_draw_text(ctx, timeChar, s_time_font, GRect(t4, y, digitWidth*2, digitWidth), overflow_mode, alignment, NULL);
    graphics_draw_text(ctx, ":", s_time_font, GRect(t5, y, digitWidth*1.5, digitWidth), overflow_mode, alignment, NULL);
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
        graphics_context_set_stroke_width(ctx, 1);
        gap = PBL_IF_ROUND_ELSE(8, 0);
        GRect mArcRect = setArcRect(px(21));
        angle = 270 - gap;
        sweep = (360 - gap - gap) * (mBattery / 100.0f);
        angle -= sweep;
        canvas_draw_arc(ctx, mArcRect, angle, sweep);

        // // battery percent
        // int percent2 = mBattery;
        // char batteryChar[5] = {0};
        // batteryChar[0] = percent2 == 100 ? '1' : ' ';
        // batteryChar[1] = percent2 >= 10 ? (char)(((percent2 % 100) / 10) + (int)'0') : ' ';
        // batteryChar[2] = (char)((percent2 % 10) + (int)'0');
        // batteryChar[3] = '%';
        // // canvas.drawText(batteryChar, 0, 4, canvas_center_x + 4, 20, mBatteryPercentPaint);
        // graphics_draw_text(ctx, batteryChar, font, box, overflow_mode, alignment, attributes);

        // if (mBatteryIcon != null) {
        //     canvas.drawBitmap(mBatteryIcon, null, mBatteryIconBounds, mIconPaint);
        // }
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
        drawComplications(ctx);
    }

    // if (!mAmbient) {
    //     // outer details
    //     drawOverlapping(ctx);
    // }
}

void face_layer_settings_changed() {
    GColor *pal = gbitmap_get_palette(s_logo_bitmap);
    pal[0] = bgColor;
    pal[1] = fgColor;
    gbitmap_set_palette(s_logo_bitmap, pal, false);
}

void face_layer_init() {
    s_logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLLOW);
#if PBL_DISPLAY_HEIGHT >= 180
    int alarm_clock_font = RESOURCE_ID_FONT_ALARM_CLOCK_50;
#else
    int alarm_clock_font = RESOURCE_ID_FONT_ALARM_CLOCK_45;
#endif
    s_time_font = fonts_load_custom_font(resource_get_handle(alarm_clock_font));
}

void face_layer_deinit() {
    gbitmap_destroy(s_logo_bitmap);
    fonts_unload_custom_font(s_time_font);
}
