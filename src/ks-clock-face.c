#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_hours_layer;
static TextLayer *s_minutes_layer;
static GFont s_font;

static void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);


    // Write the current hours and minutes into a buffer
    static char s_hours_buffer[8];
    strftime(s_hours_buffer, sizeof(s_hours_buffer), clock_is_24h_style() ? "%H" : "%I", tick_time);

    // Display this time on the TextLayer
    text_layer_set_text(s_hours_layer, s_hours_buffer);


    // Write the current hours and minutes into a buffer
    static char s_minutes_buffer[8];
    strftime(s_minutes_buffer, sizeof(s_minutes_buffer), "%M", tick_time);

    // Display this time on the TextLayer
    text_layer_set_text(s_minutes_layer, s_minutes_buffer);


    // Start vibration on each full hour (probably better way would be checking HOUR_UNIT=true)
    if (tick_time->tm_min == 0) {
        APP_LOG(APP_LOG_LEVEL_INFO, "full hour");
        vibes_short_pulse();
        light_enable_interaction();
    }
}

static void tick_minute_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void main_window_load(Window *window) {
    // Get information about the Window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_font = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
    //s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OPEN_SANS_70));


    // Create the TextLayer with specific bounds
    s_hours_layer = text_layer_create(GRect(0, 0, bounds.size.w, 84));

    // Improve the layout to be more like a watchface
    text_layer_set_background_color(s_hours_layer, GColorBlack);
    text_layer_set_text_color(s_hours_layer, GColorWhite);
    text_layer_set_font(s_hours_layer, s_font);
    text_layer_set_text_alignment(s_hours_layer, GTextAlignmentCenter);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_hours_layer));



    // Create the TextLayer with specific bounds
    s_minutes_layer = text_layer_create(GRect(0, 84, bounds.size.w, 84));

    // Improve the layout to be more like a watchface
    text_layer_set_background_color(s_minutes_layer, GColorBlack);
    text_layer_set_text_color(s_minutes_layer, GColorWhite);
    text_layer_set_font(s_minutes_layer, s_font);
    text_layer_set_text_alignment(s_minutes_layer, GTextAlignmentCenter);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_minutes_layer));
}

static void main_window_unload(Window *window) {
    // Destroy TextLayer
    text_layer_destroy(s_hours_layer);
    text_layer_destroy(s_minutes_layer);

    // Unload custom font
    fonts_unload_custom_font(s_font);
}

// ----

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

    // Destroy Window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
