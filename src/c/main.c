#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_logo_bitmap;
static TextLayer *s_date_layer_1;
static TextLayer *s_date_layer_2;
static TextLayer *s_date_layer_3;
static TextLayer *s_date_layer_4;
static Layer *s_draw_layer;
static float mCenterX;
static float mCenterY;
// static float mLastX;
static float mScale;
static bool pulsing = false;
static bool mAmbient = false;
static bool showOnlyAnims = false;
static float PI = 3.141592653589793f;
static float CIRCLE = (float)(3.141592653589793f * 2);
//static float mBattery = -1.0f;
static unsigned long now = 0;

static float math_sin(float radians) {
    float degrees = radians * 180 / PI;
    int32_t _angle = DEG_TO_TRIGANGLE(degrees);
    int32_t _ret = sin_lookup(_angle);
    float ret = _ret / (float)TRIG_MAX_RATIO;
    return ret;
}

static float math_cos(float radians) {
    float degrees = radians * 180 / PI;
    int32_t _angle = DEG_TO_TRIGANGLE(degrees);
    int32_t _ret = cos_lookup(_angle);
    float ret = _ret / (float)TRIG_MAX_RATIO;
    return ret;
}

static void update_time() {
    time_t temp = time(NULL);
    now = temp * 1000;
    struct tm *tick_time = localtime(&temp);
    static char s_time[8];
    strftime(s_time, sizeof(s_time), (clock_is_24h_style() ? "%k:%M" : "%l:%M"), tick_time);
    text_layer_set_text(s_time_layer, s_time);

    static char s_day[3];
    strftime(s_day, sizeof(s_day), "%d", tick_time);
    text_layer_set_text(s_date_layer_1, s_day);

    static char s_mon[5];
    strftime(s_mon, sizeof(s_mon), "%b", tick_time);
    text_layer_set_text(s_date_layer_2, s_mon);

    static char s_dow[5];
    strftime(s_dow, sizeof(s_dow), "%a", tick_time);
    text_layer_set_text(s_date_layer_3, s_dow);

    static char s_year[5];
    strftime(s_year, sizeof(s_year), "%Y", tick_time);
    text_layer_set_text(s_date_layer_4, s_year);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static float px(float px) {
    return px * mScale;
}

static GRect rect(int x1, int y1, int x2, int y2) {
    int x = x1;
    int y = y1;
    int w = x2 - x1;
    int h = y2 - y1;
    return GRect(x, y, w, h);
}

static GRect setArcRect(float topleft) {
    int x = topleft;
    int y = x;
    int w = px(400) - topleft - x;
    int h = px(400) - topleft - x;
    return GRect(x, y, w, h);
}

static void canvas_draw_arc(GContext *ctx, GRect rect, int32_t angle_start, int32_t angle_sweep) {
    angle_start += 90;
    GOvalScaleMode scaleMode = GOvalScaleModeFitCircle;
    int32_t angle_end = angle_start + angle_sweep;
    graphics_draw_arc(ctx, rect, scaleMode, DEG_TO_TRIGANGLE(angle_start), DEG_TO_TRIGANGLE(angle_end));
}

static void canvas_draw_line(GContext *ctx, int x1, int y1, int x2, int y2) {
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "draw line %d, %d - %d, %d", x1, y1, x2, y2);
    graphics_draw_line(ctx, GPoint(x1, y1), GPoint(x2, y2));
}

static void drawSimpleBackground(GContext *ctx) {
    int gap;
    float angle, sweep;

    // outer ring
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, px(20));
    gap = 5; // was 3 but stroke is not square?
    GRect mArcRect = setArcRect(px(38));
    canvas_draw_arc(ctx, mArcRect, 90+gap, 120-gap-gap);
    canvas_draw_arc(ctx, mArcRect, 90+gap+120, 120-gap-gap);
    canvas_draw_arc(ctx, mArcRect, 90+gap+240, 120-gap-gap);

    if (!pulsing && !showOnlyAnims) {
        // remove the contents of the outer ring segments
        graphics_context_set_stroke_color(ctx, GColorClear);
        graphics_context_set_stroke_width(ctx, px(16));
        sweep = 120 - gap - gap - 2;
        angle = 90 + gap + 1;
        canvas_draw_arc(ctx, mArcRect, angle, sweep);

        angle = 90 + gap + 120 + 1;
        canvas_draw_arc(ctx, mArcRect, angle, sweep);

        angle = 90 + gap + 240 + 1;
        canvas_draw_arc(ctx, mArcRect, angle, sweep);
    }
}

static void drawComplexBackground(GContext *ctx) {
    // ticks
    int ticks = 120;
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 1); //px(2)
    float outerTickRadius = mCenterX - px(5);
    float innerTickRadius = mCenterX - px(15);
    float one = CIRCLE / (float)ticks;

    for (int tickIndex = 0; tickIndex < ticks; tickIndex++) {
        float tickRot = (one * tickIndex);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickIndex %d", tickIndex);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickRot %d.%03d", (int)tickRot, (int)(tickRot*1000)%1000);
        float tick_innerX = math_sin(tickRot) * innerTickRadius;
        float tick_innerY = -math_cos(tickRot) * innerTickRadius;
        float tick_outerX = math_sin(tickRot) * outerTickRadius;
        float tick_outerY = -math_cos(tickRot) * outerTickRadius;
        // if (/*mBattery == -1 ||*/ showOnlyAnims || (tickIndex > 2 && tickIndex < (ticks - 2))) {
            canvas_draw_line(ctx, mCenterX + tick_innerX, mCenterY + tick_innerY,
                    mCenterX + tick_outerX, mCenterY + tick_outerY);
        // }
    }

    // active tick
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 2);
    int tickOffset = (int)((now % 60000) / 1000) * 2;
    //if (mBattery == -1 || showOnlyAnims || (tickOffset > 2 && tickOffset < (ticks - 2))) {
        float tickRot = one * tickOffset;
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickOffset %d", tickOffset);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "tickRot %d.%03d", (int)tickRot, (int)(tickRot*1000)%1000);
        float innerX = math_sin(tickRot) * innerTickRadius;
        float innerY = -math_cos(tickRot) * innerTickRadius;
        float outerX = math_sin(tickRot) * outerTickRadius;
        float outerY = -math_cos(tickRot) * outerTickRadius;
        canvas_draw_line(ctx, mCenterX + innerX, mCenterY + innerY,
                mCenterX + outerX, mCenterY + outerY);
    //}

    // int sweep;
    // if (showOnlyAnims) {
    //     // left half circle
    //     setArcRect(px(21));
    //     canvas.drawArc(mArcRect, 90, 180, false, mHalfCirclePaint);
    // } else {
    //     // outer ring details
    //     setArcRect(px(22));
    //     canvas.drawArc(mArcRect, 0, 360, false, mOuterBandDetailPaint);
    // }

    // // outer ring details 2
    // setArcRect(px(24));
    // sweep = 16;
    // canvas.drawArc(mArcRect, 90 + 60 - sweep / 2f, sweep, false, mOuterBandDetail2Paint);
    // canvas.drawArc(mArcRect, 90 + 120 + 60 - sweep / 2f, sweep, false, mOuterBandDetail2Paint);
    // canvas.drawArc(mArcRect, 90 + 240 + 60 - sweep / 2f, sweep, false, mOuterBandDetail2Paint);
    // sweep = 4;
    // canvas.drawArc(mArcRect, 90 + 2.5f, sweep, false, mOuterBandDetail3Paint);
    // canvas.drawArc(mArcRect, 90 - 2.5f - sweep, sweep, false, mOuterBandDetail3Paint);
    // canvas.drawArc(mArcRect, 90 + 120 + 2.5f, sweep, false, mOuterBandDetail3Paint);
    // canvas.drawArc(mArcRect, 90 + 120 - 2.5f - sweep, sweep, false, mOuterBandDetail3Paint);
    // canvas.drawArc(mArcRect, 90 + 240 + 2.5f, sweep, false, mOuterBandDetail3Paint);
    // canvas.drawArc(mArcRect, 90 + 240 - 2.5f - sweep, sweep, false, mOuterBandDetail3Paint);
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
    graphics_context_set_antialiased(ctx, true);
    // graphics_context_set_stroke_color(ctx, GColorClear);
    // graphics_context_set_stroke_width(ctx, 1);

    // outer band (hollow)
    drawSimpleBackground(ctx);
    if (!mAmbient) {
        // outer details (including ticks)
        drawComplexBackground(ctx);
        // inner details (animating)
        // drawAnimations(ctx);
    }

    if (!showOnlyAnims) {
        // battery band, outer band
        // drawComplications(ctx);
        // date, time, notifications
        // drawForeground(ctx);
    }

    if (!mAmbient) {
        // outer details
        // drawOverlapping(ctx);
    }

    // graphics_context_set_stroke_color(ctx, GColorClear);
    // graphics_context_set_stroke_width(ctx, 1);
}

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    mCenterX = bounds.size.w / 2.0f;
    mCenterY = bounds.size.w / 2.0f; // FIXME h > w but other parts of the code assume they are the same!
    APP_LOG(APP_LOG_LEVEL_DEBUG, "center is %d %d", (int)mCenterX, (int)mCenterY);
    // mLastX = bounds.size.w - 1;
    mScale = (bounds.size.w / 400.0f); // 1.0 on TicWatch E, 0.9 on TicWatch E3
    APP_LOG(APP_LOG_LEVEL_DEBUG, "mScale %d.%03d", (int)mScale, (int)(mScale*1000)%1000);

    s_logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLLOW);
    s_background_layer = bitmap_layer_create(rect(mCenterX - px(60), px(55), mCenterX + px(60), px(55) + px(120)));
    bitmap_layer_set_bitmap(s_background_layer, s_logo_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALARM_CLOCK_40));
    s_time_layer = text_layer_create(GRect(0, 75, bounds.size.w, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

    s_date_layer_1 = text_layer_create(GRect(20, 50, 20, 10));
    text_layer_set_background_color(s_date_layer_1, GColorClear);
    text_layer_set_text_color(s_date_layer_1, GColorWhite);
    text_layer_set_font(s_date_layer_1, fonts_get_system_font(FONT_KEY_GOTHIC_09));
    text_layer_set_text_alignment(s_date_layer_1, GTextAlignmentRight);
    text_layer_set_text(s_date_layer_1, "01");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_1));

    s_date_layer_2 = text_layer_create(GRect(20, 60, 20, 10));
    text_layer_set_background_color(s_date_layer_2, GColorClear);
    text_layer_set_text_color(s_date_layer_2, GColorWhite);
    text_layer_set_font(s_date_layer_2, fonts_get_system_font(FONT_KEY_GOTHIC_09));
    text_layer_set_text_alignment(s_date_layer_2, GTextAlignmentRight);
    text_layer_set_text(s_date_layer_2, "Jan");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_2));

    s_date_layer_3 = text_layer_create(GRect(105, 50, 20, 10));
    text_layer_set_background_color(s_date_layer_3, GColorClear);
    text_layer_set_text_color(s_date_layer_3, GColorWhite);
    text_layer_set_font(s_date_layer_3, fonts_get_system_font(FONT_KEY_GOTHIC_09));
    text_layer_set_text_alignment(s_date_layer_3, GTextAlignmentLeft);
    text_layer_set_text(s_date_layer_3, "Mon");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_3));

    s_date_layer_4 = text_layer_create(GRect(105, 60, 20, 10));
    text_layer_set_background_color(s_date_layer_4, GColorClear);
    text_layer_set_text_color(s_date_layer_4, GColorWhite);
    text_layer_set_font(s_date_layer_4, fonts_get_system_font(FONT_KEY_GOTHIC_09));
    text_layer_set_text_alignment(s_date_layer_4, GTextAlignmentLeft);
    text_layer_set_text(s_date_layer_4, "1970");
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer_4));

    s_draw_layer = layer_create(bounds);
    layer_set_update_proc(s_draw_layer, layer_update_proc);
    layer_add_child(window_layer, s_draw_layer);
}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
    fonts_unload_custom_font(s_time_font);
    gbitmap_destroy(s_logo_bitmap);
    bitmap_layer_destroy(s_background_layer);
    text_layer_destroy(s_date_layer_1);
    text_layer_destroy(s_date_layer_2);
    text_layer_destroy(s_date_layer_3);
    text_layer_destroy(s_date_layer_4);
    layer_destroy(s_draw_layer);
}

static void init() {
    s_main_window = window_create();
    window_set_background_color(s_main_window, GColorBlack);

    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    const bool animated = true;
    window_stack_push(s_main_window, animated);

    //tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    update_time();
}

static void deinit() {
    window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
