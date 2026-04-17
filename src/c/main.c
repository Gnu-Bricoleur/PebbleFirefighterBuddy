#include <pebble.h>

#define MAX_LOGS 10

static Window *s_main_window;
static TextLayer *s_counter_layer;
static TextLayer *s_time_layer;
static TextLayer *s_log_layer;
static TextLayer *s_email_layer;

static AppTimer *s_timer;
static int seconds = 0;

// Logs
static char s_logs[MAX_LOGS][32];
static int s_log_count = 0;

// Email
static bool s_email_visible = false;
static char s_email_text[128] = "";

// ---------------- COUNTER ----------------
static void timer_callback(void *data) {
  seconds++;

  int mins = seconds / 60;
  int secs = seconds % 60;

  static char buffer[16];
  snprintf(buffer, sizeof(buffer), "%02d:%02d", mins, secs);
  text_layer_set_text(s_counter_layer, buffer);

  s_timer = app_timer_register(1000, timer_callback, NULL);
}

// ---------------- TIME ----------------
static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char buffer[16];
  strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
  text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

// ---------------- LOGS ----------------
static void update_logs_display() {
  static char full_text[256];
  full_text[0] = '\0';

  for (int i = 0; i < s_log_count; i++) {
    strcat(full_text, s_logs[i]);
    strcat(full_text, "\n");
  }

  text_layer_set_text(s_log_layer, full_text);
}

// ---------------- BUTTON ----------------
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {

  // Toggle email visibility
  if (strlen(s_email_text) > 0) {
    s_email_visible = !s_email_visible;

    if (s_email_visible) {
      text_layer_set_text(s_email_layer, s_email_text);
    } else {
      text_layer_set_text(s_email_layer, "");
    }
  }

  // Log press
  if (s_log_count >= MAX_LOGS) return;

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  char time_str[8];
  strftime(time_str, sizeof(time_str), "%H:%M", tick_time);

  int mins = seconds / 60;
  int secs = seconds % 60;

  snprintf(s_logs[s_log_count], sizeof(s_logs[s_log_count]),
           "%s - %02d:%02d", time_str, mins, secs);

  s_log_count++;
  update_logs_display();
}

// ---------------- APP MESSAGE ----------------
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_find(iterator, MESSAGE_KEY_email_text);

  if (t) {
    snprintf(s_email_text, sizeof(s_email_text), "%s", t->value->cstring);

    s_email_visible = true;
    text_layer_set_text(s_email_layer, s_email_text);

    vibes_double_pulse();
  }
}

// ---------------- UI ----------------
static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_counter_layer = text_layer_create(GRect(0, 20, bounds.size.w, 60));
  text_layer_set_font(s_counter_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_counter_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_counter_layer));

  s_time_layer = text_layer_create(GRect(0, 80, bounds.size.w, 30));
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // Email preview
  s_email_layer = text_layer_create(GRect(0, 110, bounds.size.w, 40));
  text_layer_set_font(s_email_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_email_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_email_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(s_email_layer));

  // Logs
  s_log_layer = text_layer_create(GRect(0, 150, bounds.size.w, 60));
  text_layer_set_font(s_log_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_overflow_mode(s_log_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(s_log_layer));

  update_time();
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_counter_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_email_layer);
  text_layer_destroy(s_log_layer);
}

// ---------------- INIT ----------------
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void init() {
  s_main_window = window_create();

  window_set_click_config_provider(s_main_window, click_config_provider);

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });

  window_stack_push(s_main_window, true);

  s_timer = app_timer_register(1000, timer_callback, NULL);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(256, 256);
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}