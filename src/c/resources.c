#include <pebble.h>

GFont time_font;
GFont date_font;
GFont battery_font;
GBitmap *logo_bitmap;

void resources_init() {
    // package.json loads a different sized bitmap on different systems
    logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLLOW);

#if PBL_DISPLAY_HEIGHT >= 180
    // fonts for bigger screens
    int alarm_clock_font = RESOURCE_ID_FONT_ALARM_CLOCK_50;
    date_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    // chalk wants a small battery font, emery wants a big one
    battery_font = fonts_get_system_font(PBL_IF_ROUND_ELSE(FONT_KEY_GOTHIC_18_BOLD,
                                                           FONT_KEY_GOTHIC_24_BOLD));
#else
    // fonts for smaller screens
    int alarm_clock_font = RESOURCE_ID_FONT_ALARM_CLOCK_45;
    date_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    battery_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
#endif

    time_font = fonts_load_custom_font(resource_get_handle(alarm_clock_font));
}

void resources_deinit() {
    gbitmap_destroy(logo_bitmap);
    fonts_unload_custom_font(time_font);
}
