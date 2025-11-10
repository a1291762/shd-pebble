#include <pebble.h>
#include "settings.h"

GColor fgColor;
GColor bgColor;

void palette_init() {
    if (settings.InvertColor) {
        fgColor = GColorBlack;
        bgColor = GColorWhite;
    } else {
        fgColor = GColorWhite;
        bgColor = GColorBlack;
    }
}
