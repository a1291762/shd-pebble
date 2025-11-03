#include <pebble.h>
#include "math.h"

float math_sin(float radians) {
    float degrees = radians * 180 / PI;
    int32_t _angle = DEG_TO_TRIGANGLE(degrees);
    int32_t _ret = sin_lookup(_angle);
    float ret = _ret / (float)TRIG_MAX_RATIO;
    return ret;
}

float math_cos(float radians) {
    float degrees = radians * 180 / PI;
    int32_t _angle = DEG_TO_TRIGANGLE(degrees);
    int32_t _ret = cos_lookup(_angle);
    float ret = _ret / (float)TRIG_MAX_RATIO;
    return ret;
}
