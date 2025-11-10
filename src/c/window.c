#include <pebble.h>
#include "settings.h"
#include "math.h"
#include "battery.h"
#include "time.h"
#include "health.h"
#include "window.h"
#include "canvas.h"
#include "background.h"
#include "palette.h"
#include "seconds.h"

static Window *s_window;
static Layer *s_background_layer;
static Layer *s_seconds_layer;
static TextLayer *s_time_layer;
static GFont s_time_font;
// static BitmapLayer *s_background_layer;
// static GBitmap *s_logo_bitmap;
// static TextLayer *s_date_layer_1;
// static TextLayer *s_date_layer_2;
// static TextLayer *s_date_layer_3;
// static TextLayer *s_date_layer_4;
// static TextLayer *s_battery_layer;
// static TextLayer *s_step_layer;
// static TextLayer *s_minute_layer;
// static TextLayer *s_hour_layer;
// static char *smallFont = FONT_KEY_GOTHIC_18_BOLD;

static void time_changed(bool updateall) {
    // if (updateall) {
    //     text_layer_set_text(s_time_layer, s_time);
    //     text_layer_set_text(s_date_layer_1, s_day);
    //     text_layer_set_text(s_date_layer_2, s_mon);
    //     text_layer_set_text(s_date_layer_3, s_dow);
    //     text_layer_set_text(s_date_layer_4, s_year);
    // }
    if (updateall || settings.DisplaySeconds) {
        layer_mark_dirty(s_seconds_layer);
    }
}

static void battery_changed() {
    // text_layer_set_text(s_battery_layer, batteryChar);
}

static void health_changed() {
    // text_layer_set_text(s_step_layer, stepsChar);
    // text_layer_set_text(s_minute_layer, minutesChar);
    // text_layer_set_text(s_hour_layer, hoursChar);
}

static void settings_changed() {
    palette_init();
    window_set_background_color(s_window, GColorBlack);
    text_layer_set_text_color(s_time_layer, fgColor);
    // text_layer_set_text_color(s_date_layer_1, fgColor);
    // text_layer_set_text_color(s_date_layer_2, fgColor);
    // text_layer_set_text_color(s_date_layer_3, fgColor);
    // text_layer_set_text_color(s_date_layer_4, fgColor);
    // text_layer_set_text_color(s_battery_layer, fgColor);
    // text_layer_set_text_color(s_step_layer, fgColor);
    // text_layer_set_text_color(s_minute_layer, fgColor);
    // text_layer_set_text_color(s_hour_layer, fgColor);

    // GColor *pal = gbitmap_get_palette(s_logo_bitmap);
    // pal[0] = bgColor;
    // pal[1] = fgColor;
    // gbitmap_set_palette(s_logo_bitmap, pal, false);

    time_init(time_changed);
    // battery_init(battery_changed);
    // health_init(health_changed);
}

void main_window_load(Window *window) {
    s_window = window;

    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);
    canvas_init(window_bounds);

    // on rect watches, this layer is 160x160 and located partially off the screen
    s_background_layer = layer_create(canvas_bounds);
    layer_set_update_proc(s_background_layer, background_layer_update_proc);
    layer_add_child(window_layer, s_background_layer);

    // on rect watches, this layer is 160x160 and located partially off the screen
    s_seconds_layer = layer_create(canvas_bounds);
    layer_set_update_proc(s_seconds_layer, seconds_layer_update_proc);
    layer_add_child(window_layer, s_seconds_layer);

    // s_logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLLOW);
    // s_background_layer = bitmap_layer_create(GRect(screen_center_x - px(65), screen_center_y - px(135), px(120), px(120)));
    // bitmap_layer_set_background_color(s_background_layer, GColorClear);
    // bitmap_layer_set_bitmap(s_background_layer, s_logo_bitmap);
    // layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

#if PBL_DISPLAY_HEIGHT >= 180
    int alarm_clock_font = RESOURCE_ID_FONT_ALARM_CLOCK_50;
#else
    int alarm_clock_font = RESOURCE_ID_FONT_ALARM_CLOCK_45;
#endif
    s_time_font = fonts_load_custom_font(resource_get_handle(alarm_clock_font));
    s_time_layer = text_layer_create(GRect(screen_center_x - px(195), screen_center_y - px(35), px(400), px(139)));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

    // s_date_layer_1 = text_layer_create(GRect(x + canvas_center_x - px(95) - px(45), screen_center_y - px(120), px(75), px(50)));
    // text_layer_set_background_color(s_date_layer_1, GColorClear);
    // text_layer_set_font(s_date_layer_1, fonts_get_system_font(smallFont));
    // text_layer_set_text_alignment(s_date_layer_1, GTextAlignmentRight);
    // text_layer_set_text(s_date_layer_1, "01");
    // layer_add_child(window_layer, text_layer_get_layer(s_date_layer_1));

    // s_date_layer_2 = text_layer_create(GRect(x + canvas_center_x - px(95) - px(45), screen_center_y - px(80), px(75), px(50)));
    // text_layer_set_background_color(s_date_layer_2, GColorClear);
    // text_layer_set_font(s_date_layer_2, fonts_get_system_font(smallFont));
    // text_layer_set_text_alignment(s_date_layer_2, GTextAlignmentRight);
    // text_layer_set_text(s_date_layer_2, "Jan");
    // layer_add_child(window_layer, text_layer_get_layer(s_date_layer_2));

    // s_date_layer_3 = text_layer_create(GRect(x + canvas_center_x + px(85) - px(35), screen_center_y - px(120), px(75), px(50)));
    // text_layer_set_background_color(s_date_layer_3, GColorClear);
    // text_layer_set_font(s_date_layer_3, fonts_get_system_font(smallFont));
    // text_layer_set_text_alignment(s_date_layer_3, GTextAlignmentLeft);
    // text_layer_set_text(s_date_layer_3, "Mon");
    // layer_add_child(window_layer, text_layer_get_layer(s_date_layer_3));

    // s_date_layer_4 = text_layer_create(GRect(x + canvas_center_x + px(85) - px(35), screen_center_y - px(80), px(85), px(50)));
    // text_layer_set_background_color(s_date_layer_4, GColorClear);
    // text_layer_set_font(s_date_layer_4, fonts_get_system_font(smallFont));
    // text_layer_set_text_alignment(s_date_layer_4, GTextAlignmentLeft);
    // text_layer_set_text(s_date_layer_4, "1970");
    // layer_add_child(window_layer, text_layer_get_layer(s_date_layer_4));

    
    // s_battery_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(canvas_center_x - px(40), canvas_center_y - px(205), px(90), px(40)),
    //                                                       GRect(0, -5, window_bounds.size.w, px(50))));
    // text_layer_set_background_color(s_battery_layer, GColorClear);
    // text_layer_set_font(s_battery_layer, fonts_get_system_font(PBL_IF_ROUND_ELSE(FONT_KEY_GOTHIC_14, smallFont)));
    // text_layer_set_text_alignment(s_battery_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    // text_layer_set_text(s_battery_layer, "100%");
    // layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));

    // s_step_layer = text_layer_create(GRect(0, window_bounds.size.h - px(50), window_bounds.size.w, px(50)));
    // text_layer_set_background_color(s_step_layer, GColorClear);
    // text_layer_set_font(s_step_layer, fonts_get_system_font(smallFont));
    // text_layer_set_text_alignment(s_step_layer, GTextAlignmentRight);
    // text_layer_set_text(s_step_layer, "5 mins");
    // layer_add_child(window_layer, text_layer_get_layer(s_step_layer));

    // s_minute_layer = text_layer_create(GRect(0, window_bounds.size.h - px(50), window_bounds.size.w, px(50)));
    // text_layer_set_background_color(s_minute_layer, GColorClear);
    // text_layer_set_font(s_minute_layer, fonts_get_system_font(smallFont));
    // text_layer_set_text_alignment(s_minute_layer, GTextAlignmentLeft);
    // text_layer_set_text(s_minute_layer, "steps 1000");
    // layer_add_child(window_layer, text_layer_get_layer(s_minute_layer));

    // s_hour_layer = text_layer_create(GRect(0, -5, window_bounds.size.w, px(50)));
    // text_layer_set_background_color(s_hour_layer, GColorClear);
    // text_layer_set_font(s_hour_layer, fonts_get_system_font(smallFont));
    // text_layer_set_text_alignment(s_hour_layer, GTextAlignmentLeft);
    // text_layer_set_text(s_hour_layer, "1 hour");
    // layer_add_child(window_layer, text_layer_get_layer(s_hour_layer));

    settings_init(settings_changed);
}

void main_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
    fonts_unload_custom_font(s_time_font);
    // gbitmap_destroy(s_logo_bitmap);
    // bitmap_layer_destroy(s_background_layer);
    // text_layer_destroy(s_date_layer_1);
    // text_layer_destroy(s_date_layer_2);
    // text_layer_destroy(s_date_layer_3);
    // text_layer_destroy(s_date_layer_4);
    // text_layer_destroy(s_battery_layer);
    // text_layer_destroy(s_step_layer);
    // text_layer_destroy(s_minute_layer);
    // text_layer_destroy(s_hour_layer);
    layer_destroy(s_seconds_layer);
    layer_destroy(s_background_layer);
    settings_deinit();
    // time_deinit();
    // battery_deinit();
    // health_deinit();
}
