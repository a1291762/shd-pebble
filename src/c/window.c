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

static void time_changed() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "redraw (time)");
    layer_mark_dirty(s_face_layer);
}

static void battery_changed() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "redraw (battery)");
    layer_mark_dirty(s_face_layer);
}

static void health_changed() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "redraw (health)");
    layer_mark_dirty(s_face_layer);
}

static void settings_changed() {
    palette_init();
    window_set_background_color(s_window, windowColor);

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

    PBL_IF_RECT_ELSE({
        // on rect watches, this layer matches the screen
        s_ext_layer = layer_create(window_bounds);
        layer_set_update_proc(s_ext_layer, ext_layer_update_proc);
        layer_add_child(window_layer, s_ext_layer);
    }, {});

    settings_init(settings_changed);
}

void main_window_unload(Window *window) {
    layer_destroy(s_face_layer);
    PBL_IF_RECT_ELSE({
        layer_destroy(s_ext_layer);
    }, {});
    face_layer_deinit();
    settings_deinit();
    time_deinit();
    battery_deinit();
    health_deinit();
}
