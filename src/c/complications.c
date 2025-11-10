#include <pebble.h>
#include "settings.h"
#include "math.h"
#include "battery.h"
#include "time.h"
#include "health.h"
#include "canvas.h"
#include "palette.h"

static GFont s_time_font;
static bool pulsing = false;
static bool mAmbient = false;
static bool showOnlyAnims = false;
static char *smallFont = FONT_KEY_GOTHIC_18_BOLD;

static void drawComplications(GContext *ctx) {
    int gap;
    float angle, sweep, percent;

    if (mBattery != -1) {
        // battery ring
        graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(GColorGreen, fgColor));
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
        graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(GColorOrange, fgColor));
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

void date_layer_update_proc(Layer *layer, GContext *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "update date layer");
    // date
    GFont date_font = fonts_get_system_font(smallFont);
    GTextOverflowMode overflow_mode = GTextOverflowModeWordWrap;
    GTextAlignment alignment = GTextAlignmentRight;
    GSize textSize = graphics_text_layout_get_content_size("8", date_font, GRect(0, 0, 100, 100), overflow_mode, alignment);
    float digitWidth = textSize.w;
    float x1 = px(55);
    float y = px(70);
    float y2 = y + 0.8*textSize.h;

    graphics_context_set_text_color(ctx, fgColor);
    if (s_day[0] == '0') {
        s_day[0] = ' ';
    }
    graphics_draw_text(ctx, s_day, date_font, GRect(x1, y, digitWidth*4, digitWidth), overflow_mode, alignment, NULL);
    graphics_draw_text(ctx, s_mon, date_font, GRect(x1, y2, digitWidth*4, digitWidth), overflow_mode, alignment, NULL);

    alignment = GTextAlignmentLeft;
    x1 = canvas_center_x + px(45);
    graphics_draw_text(ctx, s_dow, date_font, GRect(x1, y, digitWidth*5, digitWidth), overflow_mode, alignment, NULL);
    graphics_draw_text(ctx, s_year, date_font, GRect(x1, y2, digitWidth*5, digitWidth), overflow_mode, alignment, NULL);
}

void time_layer_update_proc(Layer *layer, GContext *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "update time layer");
    GTextOverflowMode overflow_mode = GTextOverflowModeWordWrap;
    GTextAlignment alignment = GTextAlignmentLeft;
    GSize textSize = graphics_text_layout_get_content_size("8", s_time_font, GRect(0, 0, 100, 100), overflow_mode, alignment);
    float digitWidth = textSize.w;
    float x1 = canvas_center_x - 2.1*digitWidth;
    float x2 = x1 + 3.5*digitWidth;
    float y = canvas_center_y - digitWidth * 0.7;
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

void complications_layer_update_proc(Layer *layer, GContext *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "update complications layer");
    drawComplications(ctx);
}

void complications_init() {
#if PBL_DISPLAY_HEIGHT >= 180
    int alarm_clock_font = RESOURCE_ID_FONT_ALARM_CLOCK_50;
#else
    int alarm_clock_font = RESOURCE_ID_FONT_ALARM_CLOCK_45;
#endif
    s_time_font = fonts_load_custom_font(resource_get_handle(alarm_clock_font));
}

void complications_deinit() {
    fonts_unload_custom_font(s_time_font);
}

void ext_complications_layer_update_proc(Layer *layer, GContext *ctx) {
}
