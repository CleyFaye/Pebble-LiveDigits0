/* GENERATED SOURCE FILE DO NOT MODIFY */
#include <pebble.h>
#include "config.h"
#define CONFIG 0
#define ANIM_ON_LOAD 10
#define ANIM_ON_SHAKE 11
#define ANIM_ON_TIME 8
#define ANIM_SPEED_FORCED 13
#define ANIM_SPEED_NORMAL 9
#define BASE_LAYOUT 3
#define BATTERY_POSITION 21
#define BATTERY_STYLE 22
#define BLUETOOTH_POSITION 19
#define BLUETOOTH_STYLE 20
#define DATE_POSITION 15
#define DATE_STYLE 16
#define DEMO_MODE 6
#define DISPLAY_WIDGETS 14
#define FORCE_LIGHT 12
#define HOUR_POSITION 1
#define INVERT_COLORS 4
#define MINUTES_POSITION 2
#define SECONDS_POSITION 17
#define SECONDS_STYLE 18
#define SKIP_DIGITS 7
#define VIBRATE_EVERY 5

static config_callback_t config_callback;
static void* config_callback_data;
static int hour_position_value = 0;
static int minutes_position_value = 1;
static int base_layout_value = 0;
static bool invert_colors_value = false;
static int vibrate_every_value = 0;
static bool demo_mode_value = false;
static bool skip_digits_value = true;
static bool anim_on_time_value = true;
static int anim_speed_normal_value = 1;
static bool anim_on_load_value = false;
static bool anim_on_shake_value = false;
static bool force_light_value = false;
static int anim_speed_forced_value = 3;
static int display_widgets_value = 0;
static int date_position_value = 2;
static int date_style_value = 1;
static int seconds_position_value = 4;
static int seconds_style_value = 1;
static int bluetooth_position_value = 1;
static int bluetooth_style_value = 3;
static int battery_position_value = 0;
static int battery_style_value = 2;
static void in_recv_handler(DictionaryIterator* iterator, void* context);
static void update_config();
static bool cfg_handle_app_message(Tuple* tuple);
static int str_to_int(const char* str);
static void in_recv_handler(DictionaryIterator* iterator, void* context)
{
Tuple* tuple = dict_read_first(iterator);
bool handled_a_value = false;
while (tuple) {
handled_a_value |= cfg_handle_app_message(tuple);
tuple = dict_read_next(iterator);
}
if (handled_a_value) { update_config(); }
}
static int str_to_int(const char* str)
{
size_t len = strlen(str);
int res = 0;
for (size_t i = 0; i < len; ++i) {
res += str[i] - '0';
}
return res;
}
static void update_config(void) { if (config_callback) { config_callback(config_callback_data); } }
int cfg_get_hour_position(void) { return hour_position_value; }
int cfg_get_minutes_position(void) { return minutes_position_value; }
int cfg_get_base_layout(void) { return base_layout_value; }
bool cfg_get_invert_colors(void) { return invert_colors_value; }
int cfg_get_vibrate_every(void) { return vibrate_every_value; }
bool cfg_get_demo_mode(void) {
    if (demo_mode_value) {
        demo_mode_value = false;
        persist_write_bool(DEMO_MODE, false);
        return true;
    } else return false;
}
bool cfg_get_skip_digits(void) { return skip_digits_value; }
bool cfg_get_anim_on_time(void) { return anim_on_time_value; }
int cfg_get_anim_speed_normal(void) { return anim_speed_normal_value; }
bool cfg_get_anim_on_load(void) { return anim_on_load_value; }
bool cfg_get_anim_on_shake(void) { return anim_on_shake_value; }
bool cfg_get_force_light(void) { return force_light_value; }
int cfg_get_anim_speed_forced(void) { return anim_speed_forced_value; }
int cfg_get_display_widgets(void) { return display_widgets_value; }
int cfg_get_date_position(void) { return date_position_value; }
int cfg_get_date_style(void) { return date_style_value; }
int cfg_get_seconds_position(void) { return seconds_position_value; }
int cfg_get_seconds_style(void) { return seconds_style_value; }
int cfg_get_bluetooth_position(void) { return bluetooth_position_value; }
int cfg_get_bluetooth_style(void) { return bluetooth_style_value; }
int cfg_get_battery_position(void) { return battery_position_value; }
int cfg_get_battery_style(void) { return battery_style_value; }
void cfg_init(config_callback_t callback, void* data)
{
config_callback = callback;
config_callback_data = data;
if (persist_exists(HOUR_POSITION)) { hour_position_value = persist_read_int(HOUR_POSITION); }
if (persist_exists(MINUTES_POSITION)) { minutes_position_value = persist_read_int(MINUTES_POSITION); }
if (persist_exists(BASE_LAYOUT)) { base_layout_value = persist_read_int(BASE_LAYOUT); }
if (persist_exists(INVERT_COLORS)) { invert_colors_value = persist_read_bool(INVERT_COLORS); }
if (persist_exists(VIBRATE_EVERY)) { vibrate_every_value = persist_read_int(VIBRATE_EVERY); }
if (persist_exists(DEMO_MODE)) { demo_mode_value = persist_read_bool(DEMO_MODE); }
if (persist_exists(SKIP_DIGITS)) { skip_digits_value = persist_read_bool(SKIP_DIGITS); }
if (persist_exists(ANIM_ON_TIME)) { anim_on_time_value = persist_read_bool(ANIM_ON_TIME); }
if (persist_exists(ANIM_SPEED_NORMAL)) { anim_speed_normal_value = persist_read_int(ANIM_SPEED_NORMAL); }
if (persist_exists(ANIM_ON_LOAD)) { anim_on_load_value = persist_read_bool(ANIM_ON_LOAD); }
if (persist_exists(ANIM_ON_SHAKE)) { anim_on_shake_value = persist_read_bool(ANIM_ON_SHAKE); }
if (persist_exists(FORCE_LIGHT)) { force_light_value = persist_read_bool(FORCE_LIGHT); }
if (persist_exists(ANIM_SPEED_FORCED)) { anim_speed_forced_value = persist_read_int(ANIM_SPEED_FORCED); }
if (persist_exists(DISPLAY_WIDGETS)) { display_widgets_value = persist_read_int(DISPLAY_WIDGETS); }
if (persist_exists(DATE_POSITION)) { date_position_value = persist_read_int(DATE_POSITION); }
if (persist_exists(DATE_STYLE)) { date_style_value = persist_read_int(DATE_STYLE); }
if (persist_exists(SECONDS_POSITION)) { seconds_position_value = persist_read_int(SECONDS_POSITION); }
if (persist_exists(SECONDS_STYLE)) { seconds_style_value = persist_read_int(SECONDS_STYLE); }
if (persist_exists(BLUETOOTH_POSITION)) { bluetooth_position_value = persist_read_int(BLUETOOTH_POSITION); }
if (persist_exists(BLUETOOTH_STYLE)) { bluetooth_style_value = persist_read_int(BLUETOOTH_STYLE); }
if (persist_exists(BATTERY_POSITION)) { battery_position_value = persist_read_int(BATTERY_POSITION); }
if (persist_exists(BATTERY_STYLE)) { battery_style_value = persist_read_int(BATTERY_STYLE); }
app_message_register_inbox_received(in_recv_handler);
app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}
void cfg_clear(void)
{
app_message_deregister_callbacks();
}
static bool cfg_handle_app_message(Tuple* tuple)
{
switch (tuple->key) {
case HOUR_POSITION:
hour_position_value = str_to_int(tuple->value->cstring);
persist_write_int(HOUR_POSITION, hour_position_value);
return true;
case MINUTES_POSITION:
minutes_position_value = str_to_int(tuple->value->cstring);
persist_write_int(MINUTES_POSITION, minutes_position_value);
return true;
case BASE_LAYOUT:
base_layout_value = str_to_int(tuple->value->cstring);
persist_write_int(BASE_LAYOUT, base_layout_value);
return true;
case INVERT_COLORS:
invert_colors_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(INVERT_COLORS, invert_colors_value);
return true;
case VIBRATE_EVERY:
vibrate_every_value = str_to_int(tuple->value->cstring);
persist_write_int(VIBRATE_EVERY, vibrate_every_value);
return true;
case DEMO_MODE:
demo_mode_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(DEMO_MODE, demo_mode_value);
return true;
case SKIP_DIGITS:
skip_digits_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(SKIP_DIGITS, skip_digits_value);
return true;
case ANIM_ON_TIME:
anim_on_time_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(ANIM_ON_TIME, anim_on_time_value);
return true;
case ANIM_SPEED_NORMAL:
anim_speed_normal_value = str_to_int(tuple->value->cstring);
persist_write_int(ANIM_SPEED_NORMAL, anim_speed_normal_value);
return true;
case ANIM_ON_LOAD:
anim_on_load_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(ANIM_ON_LOAD, anim_on_load_value);
return true;
case ANIM_ON_SHAKE:
anim_on_shake_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(ANIM_ON_SHAKE, anim_on_shake_value);
return true;
case FORCE_LIGHT:
force_light_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(FORCE_LIGHT, force_light_value);
return true;
case ANIM_SPEED_FORCED:
anim_speed_forced_value = str_to_int(tuple->value->cstring);
persist_write_int(ANIM_SPEED_FORCED, anim_speed_forced_value);
return true;
case DISPLAY_WIDGETS:
display_widgets_value = str_to_int(tuple->value->cstring);
persist_write_int(DISPLAY_WIDGETS, display_widgets_value);
return true;
case DATE_POSITION:
date_position_value = str_to_int(tuple->value->cstring);
persist_write_int(DATE_POSITION, date_position_value);
return true;
case DATE_STYLE:
date_style_value = str_to_int(tuple->value->cstring);
persist_write_int(DATE_STYLE, date_style_value);
return true;
case SECONDS_POSITION:
seconds_position_value = str_to_int(tuple->value->cstring);
persist_write_int(SECONDS_POSITION, seconds_position_value);
return true;
case SECONDS_STYLE:
seconds_style_value = str_to_int(tuple->value->cstring);
persist_write_int(SECONDS_STYLE, seconds_style_value);
return true;
case BLUETOOTH_POSITION:
bluetooth_position_value = str_to_int(tuple->value->cstring);
persist_write_int(BLUETOOTH_POSITION, bluetooth_position_value);
return true;
case BLUETOOTH_STYLE:
bluetooth_style_value = str_to_int(tuple->value->cstring);
persist_write_int(BLUETOOTH_STYLE, bluetooth_style_value);
return true;
case BATTERY_POSITION:
battery_position_value = str_to_int(tuple->value->cstring);
persist_write_int(BATTERY_POSITION, battery_position_value);
return true;
case BATTERY_STYLE:
battery_style_value = str_to_int(tuple->value->cstring);
persist_write_int(BATTERY_STYLE, battery_style_value);
return true;
}
return false;
}

