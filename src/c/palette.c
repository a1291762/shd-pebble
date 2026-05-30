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
    if (animating) {
        if (settings.UseColor) {
            // A fixed palette
            fgColor = GColorWhite;
            bgColor = GColorBlack;
            outerRingColor = GColorOrange;
            innerBandColor = GColorBulgarianRose;
            innerBandBrightColor = GColorOrange;
            innerBandLineColor = GColorYellow;
            innerBandLineBrightColor = GColorWhite;
            blobColor = GColorOrange;
            blobBrightColor = GColorWhite;
            counterDotColor = GColorYellow;
            expandingColor = GColorOrange;
        } else {
            // Honour the InvertColor setting
            if (settings.InvertColor) {
                fgColor = GColorBlack;
                bgColor = GColorWhite;
            } else {
                fgColor = GColorWhite;
                bgColor = GColorBlack;
            }
            outerRingColor = fgColor;
            innerBandColor = bgColor;
            innerBandBrightColor = fgColor;
            innerBandLineColor = fgColor;
            innerBandLineBrightColor = fgColor;
            blobColor = bgColor;
            blobBrightColor = fgColor;
            counterDotColor = fgColor;
            expandingColor = fgColor;
        }
    } else {
        if (settings.UseColor) {
            fgColor = settings.ForegroundColor;
            bgColor = settings.BackgroundColor;
            timeBgColor = settings.TimeBackgroundColor;
            outerRingColor = settings.ActivityColor;
            batteryRingColor = settings.BatteryColor;
        } else if (settings.InvertColor) {
            fgColor = GColorBlack;
            bgColor = GColorWhite;
            outerRingColor = fgColor;
            batteryRingColor = fgColor;
        } else {
            fgColor = GColorWhite;
            bgColor = GColorBlack;
            outerRingColor = fgColor;
            batteryRingColor = fgColor;
        }
    }
    if (settings.PartialInvert) {
        windowColor = fgColor;
        extColor = bgColor;
    } else {
        windowColor = bgColor;
        extColor = fgColor;
    }

    // invert the logo bitmap if required
    GColor *pal = gbitmap_get_palette(logo_bitmap);
    pal[0] = bgColor;
    pal[1] = fgColor;
    gbitmap_set_palette(logo_bitmap, pal, false);
}
