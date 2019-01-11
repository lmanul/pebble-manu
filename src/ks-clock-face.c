#include <pebble.h>

#define TZ_COUNT 3

static Window *s_main_window;
static TextLayer *s_name_layers[TZ_COUNT];
static TextLayer *s_time_layers[TZ_COUNT];

const int LINE_HEIGHT = 30;
const int X_PADDING = 6;
const int Y_PADDING = 2;
const int TZ_VALUE_X_POS = 70;

//                         0      1      2      3      4
const char* TZ_NAMES[] = {"SFO", "NYC", "UTC", "BKK", "TOK"};
const int TZ_OFFSETS[] = {-8,    -5,    0,     7,     9};

int get_day_offset(struct tm utc_tm, int tz_offset) {
  return 0;
}

static struct tm get_local_time(struct tm utc_tm, int tz_offset) {
  struct tm target;
  target.tm_min = utc_tm.tm_min;
  int offset_time = utc_tm.tm_hour + tz_offset;
  if (offset_time < 0) {
    offset_time += 24;
  }
  target.tm_hour = offset_time % 24;
  return target;
}

static void update_time() {
    time_t current_time = time(NULL);
    struct tm utc_tm = *gmtime(&current_time);

    struct tm local_times[TZ_COUNT];
    static char s_time_buffers[TZ_COUNT][8];

    for (int i = 0; i < TZ_COUNT; i++) {
      local_times[i] = get_local_time(utc_tm, TZ_OFFSETS[i]);
      strftime(s_time_buffers[i], sizeof(s_time_buffers[0]), "%H:%M", &local_times[i]);
      text_layer_set_text(s_name_layers[i], TZ_NAMES[i]);
      text_layer_set_text(s_time_layers[i], s_time_buffers[i]);
    }
}

static void tick_minute_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void apply_style(TextLayer* t, bool bold) {
  text_layer_set_background_color(t, GColorWhite);
  text_layer_set_text_color(t, GColorBlack);
  text_layer_set_font(t, fonts_get_system_font(
      bold ? FONT_KEY_GOTHIC_28_BOLD : FONT_KEY_GOTHIC_28));
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  const int name_width = TZ_VALUE_X_POS - X_PADDING;

  for (int i = 0; i < TZ_COUNT; i++) {
    s_name_layers[i] = text_layer_create(GRect(
        0, i * (LINE_HEIGHT + Y_PADDING), name_width, LINE_HEIGHT));
    s_time_layers[i] = text_layer_create(GRect(
        TZ_VALUE_X_POS, i * (LINE_HEIGHT + Y_PADDING), bounds.size.w - TZ_VALUE_X_POS, LINE_HEIGHT));
  }

  for (int i = 0; i < TZ_COUNT; i++) {
    apply_style(s_name_layers[i], i == 2 /* bold */);
    text_layer_set_text_alignment(s_name_layers[i], GTextAlignmentRight);

    apply_style(s_time_layers[i], i == 2 /* bold */);
    text_layer_set_text_alignment(s_time_layers[i], GTextAlignmentLeft);

    layer_add_child(window_layer, text_layer_get_layer(s_name_layers[i]));
    layer_add_child(window_layer, text_layer_get_layer(s_time_layers[i]));
  }
}

static void main_window_unload(Window *window) {
    for (int i = 0; i < TZ_COUNT; i++) {
      text_layer_destroy(s_name_layers[i]);
      text_layer_destroy(s_time_layers[i]);
  }
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
