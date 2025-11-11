#include <pebble.h>
#include "settings.h"

GColor fgColor;
GColor bgColor;
GColor outerRingColor;
GColor batteryRingColor;

void palette_init() {
    if (settings.InvertColor) {
        fgColor = GColorBlack;
        bgColor = GColorWhite;
    } else {
        fgColor = GColorWhite;
        bgColor = GColorBlack;
    }
    outerRingColor = PBL_IF_COLOR_ELSE(GColorOrange, fgColor);
    batteryRingColor = PBL_IF_COLOR_ELSE(GColorGreen, fgColor);
}
