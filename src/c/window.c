#include <pebble.h>
#include "settings.h"
#include "math.h"
#include "battery.h"
#include "time.h"
#include "health.h"
#include "window.h"
#include "canvas.h"
#include "face.h"
#include "palette.h"
#include "ext.h"

static Window *s_window;
static Layer *s_face_layer;
static Layer *s_ext_layer;

// static TextLayer *s_battery_layer;
// static TextLayer *s_step_layer;
// static TextLayer *s_minute_layer;
// static TextLayer *s_hour_layer;
// static char *smallFont = FONT_KEY_GOTHIC_18_BOLD;

static void time_changed() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "time_changed");
    layer_mark_dirty(s_face_layer);
}

static void battery_changed() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "battery_changed");
    layer_mark_dirty(s_face_layer);
}

static void health_changed() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "health_changed");
    layer_mark_dirty(s_face_layer);
}

static void settings_changed() {
    palette_init();
    window_set_background_color(s_window, windowColor);
    face_layer_settings_changed();

    time_init(time_changed);
    battery_init(battery_changed);
    health_init(health_changed);
}

void main_window_load(Window *window) {
    s_window = window;

    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);
    canvas_init(window_bounds);

    // on rect watches, this layer is 160x160 and located partially off the screen
    s_face_layer = layer_create(canvas_bounds);
    layer_set_update_proc(s_face_layer, face_layer_update_proc);
    layer_add_child(window_layer, s_face_layer);
    face_layer_init();

    // on rect watches, this layer matches the screen
    s_ext_layer = layer_create(window_bounds);
    layer_set_update_proc(s_ext_layer, ext_layer_update_proc);
    layer_add_child(window_layer, s_ext_layer);
    ext_layer_init();

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
    layer_destroy(s_face_layer);
    layer_destroy(s_ext_layer);
    face_layer_deinit();
    settings_deinit();
    time_deinit();
    battery_deinit();
    health_deinit();
}
