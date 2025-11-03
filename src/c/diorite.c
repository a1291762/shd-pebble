#include <pebble.h>
#include "settings.h"
#include "math.h"
#include "battery.h"
#include "time.h"
#include "health.h"

static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_logo_bitmap;
static TextLayer *s_date_layer_1;
static TextLayer *s_date_layer_2;
static TextLayer *s_date_layer_3;
static TextLayer *s_date_layer_4;
static TextLayer *s_battery_layer;
static TextLayer *s_step_layer;
static TextLayer *s_minute_layer;
static Layer *s_draw_layer;
static float mCenterX;
static float mCenterY;
static float mScale;
static bool pulsing = false;
static bool mAmbient = false;
static bool showOnlyAnims = false;

static float px(float px) {
    return px * mScale;
}

static GRect setArcRect(float topleft) {
    int x = mCenterX - px(200) + topleft;
    int y = mCenterY - px(200) + topleft;
    int w = px(400) - topleft - x;
    int h = px(400) - topleft - y;
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "setArcRect %d.%03d -> %d, %d, %d, %d", (int)topleft, (int)(topleft * 1000)%1000, x, y, w, h);
    return GRect(x, y, w, h);
}

static void canvas_draw_arc(GContext *ctx, GRect rect, int32_t angle_start, int32_t angle_sweep) {
    angle_start += 90;
    GOvalScaleMode scaleMode = GOvalScaleModeFitCircle;
    int32_t angle_end = angle_start + angle_sweep;
    graphics_draw_arc(ctx, rect, scaleMode, DEG_TO_TRIGANGLE(angle_start), DEG_TO_TRIGANGLE(angle_end));
}

static void canvas_draw_line(GContext *ctx, int x1, int y1, int x2, int y2) {
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "draw line %d, %d - %d, %d", x1, y1, x2, y2);
    graphics_draw_line(ctx, GPoint(x1, y1), GPoint(x2, y2));
}

static void drawSimpleBackground(GContext *ctx) {
    int gap;
    float angle, sweep;

    // outer ring
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, px(20));
    gap = 5; // was 3 but stroke is not square?
    GRect mArcRect = setArcRect(px(38));
    canvas_draw_arc(ctx, mArcRect, 90+gap, 120-gap-gap);
    canvas_draw_arc(ctx, mArcRect, 90+gap+120, 120-gap-gap);
    canvas_draw_arc(ctx, mArcRect, 90+gap+240, 120-gap-gap);

    if (!pulsing && !showOnlyAnims) {
        // remove the contents of the outer ring segments
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_context_set_stroke_width(ctx, px(16));
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
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 1); //px(2)
    float outerTickRadius = mCenterX - px(5);
    float innerTickRadius = mCenterX - px(15);
    float one = CIRCLE / (float)ticks;

    for (int tickIndex = 0; tickIndex < ticks; tickIndex++) {
        float tickRot = (one * tickIndex);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickIndex %d", tickIndex);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickRot %d.%03d", (int)tickRot, (int)(tickRot*1000)%1000);
        float tick_innerX = math_sin(tickRot) * innerTickRadius;
        float tick_innerY = -math_cos(tickRot) * innerTickRadius;
        float tick_outerX = math_sin(tickRot) * outerTickRadius;
        float tick_outerY = -math_cos(tickRot) * outerTickRadius;
        //if (mBattery == -1 || showOnlyAnims || (tickIndex > 1 && tickIndex < (ticks - 1))) {
            canvas_draw_line(ctx, mCenterX + tick_innerX, mCenterY + tick_innerY,
                    mCenterX + tick_outerX, mCenterY + tick_outerY);
        //}
    }

    if (settings.DisplaySeconds) {
        // active tick
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_stroke_width(ctx, 2);
        int tickOffset = (int)((now % 60000) / 1000) * (ticks/60);
        //if (mBattery == -1 || showOnlyAnims || (tickOffset > 1 && tickOffset < (ticks - 1))) {
            float tickRot = one * tickOffset;
            // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickOffset %d", tickOffset);
            // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickRot %d.%03d", (int)tickRot, (int)(tickRot*1000)%1000);
            float innerX = math_sin(tickRot) * innerTickRadius;
            float innerY = -math_cos(tickRot) * innerTickRadius;
            float outerX = math_sin(tickRot) * outerTickRadius;
            float outerY = -math_cos(tickRot) * outerTickRadius;
            canvas_draw_line(ctx, mCenterX + innerX, mCenterY + innerY,
                    mCenterX + outerX, mCenterY + outerY);
        //}
    }

    // int sweep;
    // if (showOnlyAnims) {
    //     // left half circle
    //     graphics_context_set_stroke_color(ctx, GColorWhite);
    //     graphics_context_set_stroke_width(ctx, 1);
    //     GRect mArcRect = setArcRect(px(21));
    //     canvas_draw_arc(ctx, mArcRect, 90, 180);
    // } else {
    //     // outer ring details
    //     graphics_context_set_stroke_color(ctx, GColorWhite);
    //     graphics_context_set_stroke_width(ctx, 1);
    //     GRect mArcRect = setArcRect(px(22));
    //     canvas_draw_arc(ctx, mArcRect, 0, 360);
    // }

    // // outer ring details 2
    // graphics_context_set_stroke_color(ctx, GColorWhite);
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

static void drawComplications(GContext *ctx) {
    int gap;
    float angle, sweep; //, percent;

    if (mBattery != -1) {
        // battery ring
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_stroke_width(ctx, 1);
        gap = 8;
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
        // // canvas.drawText(batteryChar, 0, 4, mCenterX + 4, 20, mBatteryPercentPaint);
        // graphics_draw_text(ctx, batteryChar, font, box, overflow_mode, alignment, attributes);

        // if (mBatteryIcon != null) {
        //     canvas.drawBitmap(mBatteryIcon, null, mBatteryIconBounds, mIconPaint);
        // }
    }

//     if (!pulsing) {
//         gap = 3;
//         setArcRect(px(38));

//         // hours draws from the middle out
//         angle = 90 + gap + 120 + 1 + 56;
//         percent = (int)(mHours * 112);
//         angle -= percent / 2f;
//         sweep = percent;
//         canvas.drawArc(mArcRect, angle, sweep, false, mOuterBandPaint);

//         // minutes fills down
//         angle = 90 + 120 - gap - 1;
//         percent = (int)(mMinutes * 112);
//         sweep = -percent;
//         canvas.drawArc(mArcRect, angle, sweep, false, mOuterBandPaint);

//         // steps fills up
//         angle = 90 + gap + 240 + 1;
//         percent = (int)(mSteps * 112);
//         sweep = percent;
//         canvas.drawArc(mArcRect, angle, sweep, false, mOuterBandPaint);
//     }

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
//         canvas.drawText(MUSIC, mCenterX, mCenterY + px(170), mIconPaint);
//     }
}

static void drawOverlapping(GContext *ctx) {
    // outer ring details 3
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 1);
    float innerTickRadius = mCenterX - px(24);
    float outerTickRadius = mCenterX - px(24) + px(8);
    float one = CIRCLE / (float)360;
    float angles[6] = {
        180 - 3,
        180 + 3,
        180 + 120 - 3,
        180 + 120 + 3,
        180 + 240 - 3,
        180 + 240 + 3,
    };
    for (int i = 0; i < 6; i++) {
        float angle = angles[i];
        float tickRot = one * angle;
        float tick_innerX = math_sin(tickRot) * innerTickRadius;
        float tick_innerY = -math_cos(tickRot) * innerTickRadius;
        float tick_outerX = math_sin(tickRot) * outerTickRadius;
        float tick_outerY = -math_cos(tickRot) * outerTickRadius;
        canvas_draw_line(ctx, mCenterX + tick_innerX, mCenterY + tick_innerY,
                mCenterX + tick_outerX, mCenterY + tick_outerY);
    }
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
    // outer band (hollow)
    drawSimpleBackground(ctx);
    if (!mAmbient) {
        // outer details (including ticks)
        drawComplexBackground(ctx);
        // inner details (animating)
        // drawAnimations(ctx);
    }

    if (!showOnlyAnims) {
        // battery band, outer band
        drawComplications(ctx);
        // date, time, notifications
        // drawForeground(ctx);
    }

    if (!mAmbient) {
        // outer details
        // drawOverlapping(ctx);
    }
}

static void time_changed() {
    text_layer_set_text(s_time_layer, s_time);
    text_layer_set_text(s_date_layer_1, s_day);
    text_layer_set_text(s_date_layer_2, s_mon);
    text_layer_set_text(s_date_layer_3, s_dow);
    text_layer_set_text(s_date_layer_4, s_year);
    layer_mark_dirty(s_draw_layer);
}

static void battery_changed() {
    text_layer_set_text(s_battery_layer, batteryChar);
}

static void health_changed() {
    text_layer_set_text(s_step_layer, stepsChar);
    text_layer_set_text(s_minute_layer, minutesChar);
}

static void settings_changed() {
    time_init(time_changed);
    battery_init(battery_changed);
    health_init(health_changed);
}

void main_window_load(Window *window) {
    window_set_background_color(window, GColorBlack);

    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);
    int x = window_bounds.origin.x;
    int y = window_bounds.origin.y;
    int w = window_bounds.size.w;
    int h = window_bounds.size.h;
    // the width is intentionally wider than the actual screen
    w = 400 * 0.4; //1.15 * w;
    // offset so the image is centered
    x -= (w - window_bounds.size.w) / 2.0;
    // heigh matches width
    h = w;
    GRect bounds = GRect(x, y, w, h);

    mCenterX = bounds.size.w / 2.0f;
    mCenterY = bounds.size.h / 2.0f;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "center is %d %d", (int)mCenterX, (int)mCenterY);
    // mLastX = bounds.size.w - 1;
    mScale = 0.4; //(bounds.size.w / 400.0f); // 1.0 on TicWatch E, 0.9 on TicWatch E3
    APP_LOG(APP_LOG_LEVEL_DEBUG, "mScale %d.%03d", (int)mScale, (int)(mScale*1000)%1000);

    s_logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLLOW);
    s_background_layer = bitmap_layer_create(GRect(x + mCenterX - px(60), mCenterY - px(145), px(120), px(120)));
    bitmap_layer_set_bitmap(s_background_layer, s_logo_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALARM_CLOCK_40));
    s_time_layer = text_layer_create(GRect(x + mCenterX - px(195), mCenterY - px(55), px(400), px(139)));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

    s_date_layer_1 = text_layer_create(GRect(x + mCenterX - px(95) - px(25), mCenterY - px(110), px(55), px(40)));
    text_layer_set_background_color(s_date_layer_1, GColorClear);
    text_layer_set_text_color(s_date_layer_1, GColorWhite);
    text_layer_set_font(s_date_layer_1, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_date_layer_1, GTextAlignmentRight);
    text_layer_set_text(s_date_layer_1, "01");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_1));

    s_date_layer_2 = text_layer_create(GRect(x + mCenterX - px(95) - px(25), mCenterY - px(80), px(55), px(40)));
    text_layer_set_background_color(s_date_layer_2, GColorClear);
    text_layer_set_text_color(s_date_layer_2, GColorWhite);
    text_layer_set_font(s_date_layer_2, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_date_layer_2, GTextAlignmentRight);
    text_layer_set_text(s_date_layer_2, "Jan");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_2));

    s_date_layer_3 = text_layer_create(GRect(x + mCenterX + px(95) - px(35), mCenterY - px(110), px(55), px(40)));
    text_layer_set_background_color(s_date_layer_3, GColorClear);
    text_layer_set_text_color(s_date_layer_3, GColorWhite);
    text_layer_set_font(s_date_layer_3, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_date_layer_3, GTextAlignmentLeft);
    text_layer_set_text(s_date_layer_3, "Mon");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_3));

    s_date_layer_4 = text_layer_create(GRect(x + mCenterX + px(95) - px(35), mCenterY - px(80), px(75), px(40)));
    text_layer_set_background_color(s_date_layer_4, GColorClear);
    text_layer_set_text_color(s_date_layer_4, GColorWhite);
    text_layer_set_font(s_date_layer_4, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_date_layer_4, GTextAlignmentLeft);
    text_layer_set_text(s_date_layer_4, "1970");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_4));

    s_battery_layer = text_layer_create(GRect(0, 0, window_bounds.size.w, px(40)));
    text_layer_set_background_color(s_battery_layer, GColorClear);
    text_layer_set_text_color(s_battery_layer, GColorWhite);
    text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
    text_layer_set_text(s_battery_layer, "100%");
    layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));

    s_step_layer = text_layer_create(GRect(0, window_bounds.size.h - px(40), window_bounds.size.w, px(40)));
    text_layer_set_background_color(s_step_layer, GColorClear);
    text_layer_set_text_color(s_step_layer, GColorWhite);
    text_layer_set_font(s_step_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_step_layer, GTextAlignmentRight);
    text_layer_set_text(s_step_layer, "5 mins");
    layer_add_child(window_layer, text_layer_get_layer(s_step_layer));

    s_minute_layer = text_layer_create(GRect(0, window_bounds.size.h - px(40), window_bounds.size.w, px(40)));
    text_layer_set_background_color(s_minute_layer, GColorClear);
    text_layer_set_text_color(s_minute_layer, GColorWhite);
    text_layer_set_font(s_minute_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_minute_layer, GTextAlignmentLeft);
    text_layer_set_text(s_minute_layer, "steps 1000");
    layer_add_child(window_layer, text_layer_get_layer(s_minute_layer));

    s_draw_layer = layer_create(bounds);
    layer_set_update_proc(s_draw_layer, layer_update_proc);
    layer_add_child(window_layer, s_draw_layer);

    settings_init(settings_changed);
}

void main_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
    fonts_unload_custom_font(s_time_font);
    gbitmap_destroy(s_logo_bitmap);
    bitmap_layer_destroy(s_background_layer);
    text_layer_destroy(s_date_layer_1);
    text_layer_destroy(s_date_layer_2);
    text_layer_destroy(s_date_layer_3);
    text_layer_destroy(s_date_layer_4);
    text_layer_destroy(s_battery_layer);
    text_layer_destroy(s_step_layer);
    layer_destroy(s_draw_layer);
    settings_deinit();
    time_deinit();
    battery_deinit();
    health_deinit();
}
