#pragma once

extern bool animating;
typedef void (*anim_callback)();

void animation_start(anim_callback callback);
void animation_stop();
