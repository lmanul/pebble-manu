#include <pebble.h>

#define TZ_COUNT 5

static Window *s_main_window;
static TextLayer *s_name_layers[TZ_COUNT];
static TextLayer *s_time_layers[TZ_COUNT];

const int LINE_HEIGHT = 30;
const int X_PADDING = 6;
const int Y_PADDING = 2;
const int TZ_VALUE_X_POS = 80;

const char* TZ_NAMES[] = {"SFO", "NYC", "UTC", "BKK", "TOK"};
const int TZ_OFFSETS[] = {-8, -5, 0, 7, 9};

static void update_time() {
    time_t current_time = time(NULL);
    struct tm *utc_tm = gmtime(&current_time);
    const int utc_hour = utc_tm->tm_hour;
    const int utc_min  = utc_tm->tm_min;

    struct tm sfo_tm;
    sfo_tm.tm_min = utc_min;
    if (utc_hour < 24 - TZ_OFFSETS[0]) {
      sfo_tm.tm_hour = utc_hour + TZ_OFFSETS[0];
    } else {
      sfo_tm.tm_hour = (utc_hour + TZ_OFFSETS[0]) % 24;
    }

    // TODO: NYC

    // UTC time: no offset.

    struct tm bkk_tm;
    bkk_tm.tm_min = utc_min;
    if (utc_hour < 24 - TZ_OFFSETS[3]) {
      bkk_tm.tm_hour = utc_hour + TZ_OFFSETS[3];
    } else {
      bkk_tm.tm_hour = (utc_hour + TZ_OFFSETS[3]) % 24;
    }

    static char s_time_buffer_sfo[8];
    static char s_time_buffer_utc[8];
    static char s_time_buffer_bkk[8];

    strftime(s_time_buffer_sfo, sizeof(s_time_buffer_sfo), "%H:%M", &sfo_tm);
    text_layer_set_text(s_name_layers[0], TZ_NAMES[0]);
    text_layer_set_text(s_time_layers[0], s_time_buffer_sfo);

    strftime(s_time_buffer_utc, sizeof(s_time_buffer_utc), "%H:%M", utc_tm);
    text_layer_set_text(s_name_layers[1], TZ_NAMES[1]);
    text_layer_set_text(s_time_layers[1], s_time_buffer_utc);

    strftime(s_time_buffer_bkk, sizeof(s_time_buffer_bkk), "%H:%M", &bkk_tm);
    text_layer_set_text(s_name_layers[2], TZ_NAMES[2]);
    text_layer_set_text(s_time_layers[2], s_time_buffer_bkk);
}

static void tick_minute_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void apply_style(TextLayer* t) {
  text_layer_set_background_color(t, GColorWhite);
  text_layer_set_text_color(t, GColorBlack);
  text_layer_set_font(t, fonts_get_system_font(FONT_KEY_GOTHIC_28));
}

static void main_window_load(Window *window) {
    // Get information about the Window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    const int name_width = TZ_VALUE_X_POS - X_PADDING;
    s_name_layers[0] = text_layer_create(GRect(
        0, 0 * (LINE_HEIGHT + Y_PADDING), name_width, LINE_HEIGHT));
    s_time_layers[0] = text_layer_create(GRect(
        TZ_VALUE_X_POS, 0 * (LINE_HEIGHT + Y_PADDING), bounds.size.w - TZ_VALUE_X_POS, LINE_HEIGHT));

    s_name_layers[1] = text_layer_create(GRect(
        0, 1 * (LINE_HEIGHT + Y_PADDING), name_width, LINE_HEIGHT));
    s_time_layers[1] = text_layer_create(GRect(
        TZ_VALUE_X_POS, 1 * (LINE_HEIGHT + Y_PADDING), bounds.size.w - TZ_VALUE_X_POS, LINE_HEIGHT));

    s_name_layers[2] = text_layer_create(GRect(
        0, 2 * (LINE_HEIGHT + Y_PADDING), name_width, LINE_HEIGHT));
    s_time_layers[2] = text_layer_create(GRect(
        TZ_VALUE_X_POS, 2 * (LINE_HEIGHT + Y_PADDING), bounds.size.w - TZ_VALUE_X_POS, LINE_HEIGHT));

    apply_style(s_name_layers[0]);
    text_layer_set_text_alignment(s_name_layers[0], GTextAlignmentRight);

    apply_style(s_time_layers[0]);
    text_layer_set_text_alignment(s_time_layers[0], GTextAlignmentLeft);

    apply_style(s_name_layers[1]);
    text_layer_set_text_alignment(s_name_layers[1], GTextAlignmentRight);

    apply_style(s_time_layers[1]);
    text_layer_set_text_alignment(s_time_layers[1], GTextAlignmentLeft);

    apply_style(s_name_layers[2]);
    text_layer_set_text_alignment(s_name_layers[2], GTextAlignmentRight);

    apply_style(s_time_layers[2]);
    text_layer_set_text_alignment(s_time_layers[2], GTextAlignmentLeft);

    layer_add_child(window_layer, text_layer_get_layer(s_name_layers[0]));
    layer_add_child(window_layer, text_layer_get_layer(s_time_layers[0]));
    layer_add_child(window_layer, text_layer_get_layer(s_name_layers[1]));
    layer_add_child(window_layer, text_layer_get_layer(s_time_layers[1]));
    layer_add_child(window_layer, text_layer_get_layer(s_name_layers[2]));
    layer_add_child(window_layer, text_layer_get_layer(s_time_layers[2]));
}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_name_layers[0]);
    text_layer_destroy(s_time_layers[0]);
    text_layer_destroy(s_name_layers[1]);
    text_layer_destroy(s_time_layers[1]);
    text_layer_destroy(s_name_layers[2]);
    text_layer_destroy(s_time_layers[2]);
}

static void init() {
    APP_LOG(APP_LOG_LEVEL_INFO, "init");

    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);

    // Make sure the time is displayed from the start
    update_time();

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_minute_handler);
}

static void deinit() {
    APP_LOG(APP_LOG_LEVEL_INFO, "deinit");
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
