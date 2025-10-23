#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_logo_bitmap;
static TextLayer *s_date_layer_1;
static TextLayer *s_date_layer_2;
static TextLayer *s_date_layer_3;
static TextLayer *s_date_layer_4;
static Layer *s_draw_layer;

static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                            "%H:%M" : "%I:%M", tick_time);
    text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);

    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorClear);
    graphics_context_set_stroke_width(ctx, 5);
    graphics_draw_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h / 2), (bounds.size.w / 2) - 5);
}

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLLOW);
    s_background_layer = bitmap_layer_create(GRect(0, 30, bounds.size.w, 60));
    bitmap_layer_set_bitmap(s_background_layer, s_logo_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALARM_CLOCK_40));
    s_time_layer = text_layer_create(GRect(0, 75, bounds.size.w, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

    s_date_layer_1 = text_layer_create(GRect(20, 50, 20, 10));
    text_layer_set_background_color(s_date_layer_1, GColorClear);
    text_layer_set_text_color(s_date_layer_1, GColorWhite);
    text_layer_set_font(s_date_layer_1, fonts_get_system_font(FONT_KEY_GOTHIC_09));
    text_layer_set_text_alignment(s_date_layer_1, GTextAlignmentRight);
    text_layer_set_text(s_date_layer_1, "23");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_1));

    s_date_layer_2 = text_layer_create(GRect(20, 60, 20, 10));
    text_layer_set_background_color(s_date_layer_2, GColorClear);
    text_layer_set_text_color(s_date_layer_2, GColorWhite);
    text_layer_set_font(s_date_layer_2, fonts_get_system_font(FONT_KEY_GOTHIC_09));
    text_layer_set_text_alignment(s_date_layer_2, GTextAlignmentRight);
    text_layer_set_text(s_date_layer_2, "Oct");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_2));

    s_date_layer_3 = text_layer_create(GRect(105, 50, 20, 10));
    text_layer_set_background_color(s_date_layer_3, GColorClear);
    text_layer_set_text_color(s_date_layer_3, GColorWhite);
    text_layer_set_font(s_date_layer_3, fonts_get_system_font(FONT_KEY_GOTHIC_09));
    text_layer_set_text_alignment(s_date_layer_3, GTextAlignmentLeft);
    text_layer_set_text(s_date_layer_3, "Thu");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_3));

    s_date_layer_4 = text_layer_create(GRect(105, 60, 20, 10));
    text_layer_set_background_color(s_date_layer_4, GColorClear);
    text_layer_set_text_color(s_date_layer_4, GColorWhite);
    text_layer_set_font(s_date_layer_4, fonts_get_system_font(FONT_KEY_GOTHIC_09));
    text_layer_set_text_alignment(s_date_layer_4, GTextAlignmentLeft);
    text_layer_set_text(s_date_layer_4, "2025");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_4));

    s_draw_layer = layer_create(bounds);
    layer_set_update_proc(s_draw_layer, layer_update_proc);
    layer_add_child(window_layer, s_draw_layer);
}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
    fonts_unload_custom_font(s_time_font);
    gbitmap_destroy(s_logo_bitmap);
    bitmap_layer_destroy(s_background_layer);
    text_layer_destroy(s_date_layer_1);
    text_layer_destroy(s_date_layer_2);
    text_layer_destroy(s_date_layer_3);
    text_layer_destroy(s_date_layer_4);
    layer_destroy(s_draw_layer);
}

static void init() {
    s_main_window = window_create();
    window_set_background_color(s_main_window, GColorBlack);

    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    const bool animated = true;
    window_stack_push(s_main_window, animated);

    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    update_time();
}

static void deinit() {
    window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
