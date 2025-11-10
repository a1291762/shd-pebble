#pragma once

void date_layer_update_proc(Layer *layer, GContext *ctx);
void time_layer_update_proc(Layer *layer, GContext *ctx);
void complications_layer_update_proc(Layer *layer, GContext *ctx);
void complications_init();
void complications_deinit();

void ext_complications_layer_update_proc(Layer *layer, GContext *ctx);
