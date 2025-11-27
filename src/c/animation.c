#include <pebble.h>
#include "animation.h"
#include "time.h"
#include "palette.h"

bool animating = false;

static Animation *s_anim;
static anim_callback s_anim_callback;
static int anim_secs = 10;

static void anim_setup(Animation *animation) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "setup");
    animating = true;
    palette_init();
    //s_anim_callback();
}

static void anim_update(Animation *animation, const AnimationProgress progress) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "update");

    time_t this_seconds;
    uint16_t this_millis;
    time_ms(&this_seconds, &this_millis);
    // code ported from Java where time is expressed in milliseconds
    now = (long long)this_seconds * 1000 + this_millis;

    s_anim_callback();
}

static void anim_teardown(Animation *animation) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "teardown");
    s_anim = NULL;
    animating = false;
    palette_init();
    s_anim_callback();
}

static AnimationImplementation s_anim_impl = {
    .setup = anim_setup,
    .update = anim_update,
    .teardown = anim_teardown,
};

void animation_start(anim_callback callback) {
    if (s_anim) {
        animation_stop();
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "start");
    s_anim_callback = callback;
    s_anim = animation_create();
    animation_set_duration(s_anim, anim_secs * 1000);
    animation_set_implementation(s_anim, &s_anim_impl);
    animation_schedule(s_anim);
}

void animation_stop() {
    if (s_anim) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "stop");
        animation_unschedule(s_anim);
    }
}
