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
    bool invert = settings.InvertColor && PBL_IF_COLOR_ELSE(!animating, true);
    if (invert) {
        fgColor = GColorBlack;
        bgColor = GColorWhite;
        timeBgColor = bgColor; //GColorLightGray;
    } else {
        fgColor = GColorWhite;
        bgColor = GColorBlack;
        timeBgColor = bgColor; //GColorDarkGray;
    }
    if (settings.PartialInvert) {
        windowColor = fgColor;
        extColor = bgColor;
    } else {
        windowColor = bgColor;
        extColor = fgColor;
    }
    outerRingColor = settings.UseColor ? GColorOrange : fgColor;
    batteryRingColor = settings.UseColor ? GColorGreen : fgColor;
    innerBandColor = settings.UseColor ? GColorBulgarianRose : bgColor;
    innerBandBrightColor = settings.UseColor ? GColorOrange : fgColor;
    innerBandLineColor = settings.UseColor ? GColorYellow : fgColor;
    innerBandLineBrightColor = settings.UseColor ? GColorWhite : fgColor;
    blobColor = settings.UseColor ? GColorOrange : bgColor;
    blobBrightColor = settings.UseColor ? GColorWhite : fgColor;
    counterDotColor = settings.UseColor ? GColorYellow : fgColor;
    expandingColor = settings.UseColor ? GColorOrange : fgColor;

    // invert the logo bitmap if required
    GColor *pal = gbitmap_get_palette(logo_bitmap);
    pal[0] = bgColor;
    pal[1] = fgColor;
    gbitmap_set_palette(logo_bitmap, pal, false);
}
