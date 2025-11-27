#pragma once

typedef void (*timer_callback)();

void timer_init(timer_callback callback, uint32_t millis);
void timer_deinit();
