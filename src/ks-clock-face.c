#include <pebble.h>

#define TZ_COUNT 1

static Window *s_main_window;
static TextLayer *s_name_layer;
static TextLayer *s_time_layer;
static GFont s_font;

const int LINE_HEIGHT = 40;
const int TZ_VALUE_X_POS = 50;

const char* TZ_NAMES[] = {"UTC1", "UTC2", "UTC3", "UTC4", "UTC5"};

static void update_time() {
    time_t current_time = time(NULL);
    struct tm *utc_tm = gmtime(&current_time);

    // Write the current hours and minutes into a buffer
    static char s_hours_buffer[8];
    strftime(s_hours_buffer, sizeof(s_hours_buffer), "%H:%M", utc_tm);

    text_layer_set_text(s_time_layer, s_hours_buffer);
    text_layer_set_text(s_name_layer, TZ_NAMES[0]);
}

static void tick_minute_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void main_window_load(Window *window) {
    // Get information about the Window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    s_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);

    s_name_layer = text_layer_create(GRect(0, 0, TZ_VALUE_X_POS, LINE_HEIGHT));
    s_time_layer = text_layer_create(GRect(TZ_VALUE_X_POS, 0, bounds.size.w - TZ_VALUE_X_POS, LINE_HEIGHT));

    text_layer_set_background_color(s_name_layer, GColorWhite);
    text_layer_set_text_color(s_name_layer, GColorBlack);
    text_layer_set_font(s_name_layer, s_font);
    text_layer_set_text_alignment(s_name_layer, GTextAlignmentRight);

    text_layer_set_background_color(s_time_layer, GColorWhite);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_font(s_time_layer, s_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_name_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
    // Destroy TextLayer
    text_layer_destroy(s_name_layer);
    text_layer_destroy(s_time_layer);

    // Unload custom font
    fonts_unload_custom_font(s_font);
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
