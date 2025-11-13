#include <pebble.h>
#include "settings.h"
#include "resources.h"

GColor fgColor;
GColor bgColor;
GColor outerRingColor;
GColor batteryRingColor;
GColor windowColor;
GColor extColor;
GColor timeBgColor;

void palette_init() {
    if (settings.InvertColor) {
        fgColor = GColorBlack;
        bgColor = GColorWhite;
        timeBgColor = GColorLightGray;
    } else {
        fgColor = GColorWhite;
        bgColor = GColorBlack;
        timeBgColor = GColorDarkGray;
    }
    if (settings.PartialInvert) {
        windowColor = fgColor;
        extColor = bgColor;
    } else {
        windowColor = bgColor;
        extColor = fgColor;
    }
    outerRingColor = PBL_IF_COLOR_ELSE(GColorOrange, fgColor);
    batteryRingColor = PBL_IF_COLOR_ELSE(GColorGreen, fgColor);

    // invert the logo bitmap if required
    GColor *pal = gbitmap_get_palette(logo_bitmap);
    pal[0] = bgColor;
    pal[1] = fgColor;
    gbitmap_set_palette(logo_bitmap, pal, false);
}
