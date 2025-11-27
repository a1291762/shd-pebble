#include <pebble.h>
#include "settings.h"
#include "resources.h"
#include "animation.h"

GColor fgColor;
GColor bgColor;
GColor outerRingColor;
GColor batteryRingColor;
GColor windowColor;
GColor extColor;
GColor timeBgColor;
GColor innerBandColor;
GColor innerBandBrightColor;
GColor innerBandLineColor;
GColor innerBandLineBrightColor;
GColor blobColor;
GColor blobBrightColor;
GColor counterDotColor;
GColor expandingColor;

void palette_init() {
    bool invert = settings.InvertColor && !animating;
    if (invert) {
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
    innerBandColor = PBL_IF_COLOR_ELSE(GColorBulgarianRose, bgColor);
    innerBandBrightColor = PBL_IF_COLOR_ELSE(GColorOrange, fgColor);
    innerBandLineColor = PBL_IF_COLOR_ELSE(GColorYellow, fgColor);
    innerBandLineBrightColor = PBL_IF_COLOR_ELSE(GColorWhite, fgColor);
    blobColor = PBL_IF_COLOR_ELSE(GColorOrange, bgColor);
    blobBrightColor = PBL_IF_COLOR_ELSE(GColorWhite, fgColor);
    counterDotColor = PBL_IF_COLOR_ELSE(GColorYellow, fgColor);
    expandingColor = PBL_IF_COLOR_ELSE(GColorOrange, fgColor);

    // invert the logo bitmap if required
    GColor *pal = gbitmap_get_palette(logo_bitmap);
    pal[0] = bgColor;
    pal[1] = fgColor;
    gbitmap_set_palette(logo_bitmap, pal, false);
}
