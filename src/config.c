/* GENERATED SOURCE FILE DO NOT MODIFY */
#include <pebble.h>
#include "config.h"
#define CONFIG 0
#define ANIM_ON_LOAD 4
#define ANIM_ON_SHAKE 5
#define ANIM_ON_TIME 3
#define INVERT_COLORS 1
#define SKIP_DIGITS 2

static config_callback_t config_callback;
static void* config_callback_data;
static bool invert_colors_value = false;
static bool skip_digits_value = true;
static bool anim_on_time_value = true;
static bool anim_on_load_value = false;
static bool anim_on_shake_value = false;
static void in_recv_handler(DictionaryIterator* iterator, void* context);
static void update_config();
static bool cfg_handle_app_message(Tuple* tuple);
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
static void update_config(void) { if (config_callback) { config_callback(config_callback_data); } }
bool cfg_get_invert_colors(void) { return invert_colors_value; }
bool cfg_get_skip_digits(void) { return skip_digits_value; }
bool cfg_get_anim_on_time(void) { return anim_on_time_value; }
bool cfg_get_anim_on_load(void) { return anim_on_load_value; }
bool cfg_get_anim_on_shake(void) { return anim_on_shake_value; }
void cfg_init(config_callback_t callback, void* data)
{
config_callback = callback;
config_callback_data = data;
if (persist_exists(INVERT_COLORS)) { invert_colors_value = persist_read_bool(INVERT_COLORS); }
if (persist_exists(SKIP_DIGITS)) { skip_digits_value = persist_read_bool(SKIP_DIGITS); }
if (persist_exists(ANIM_ON_TIME)) { anim_on_time_value = persist_read_bool(ANIM_ON_TIME); }
if (persist_exists(ANIM_ON_LOAD)) { anim_on_load_value = persist_read_bool(ANIM_ON_LOAD); }
if (persist_exists(ANIM_ON_SHAKE)) { anim_on_shake_value = persist_read_bool(ANIM_ON_SHAKE); }
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
case INVERT_COLORS:
invert_colors_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(INVERT_COLORS, invert_colors_value);
return true;
case SKIP_DIGITS:
skip_digits_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(SKIP_DIGITS, skip_digits_value);
return true;
case ANIM_ON_TIME:
anim_on_time_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(ANIM_ON_TIME, anim_on_time_value);
return true;
case ANIM_ON_LOAD:
anim_on_load_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(ANIM_ON_LOAD, anim_on_load_value);
return true;
case ANIM_ON_SHAKE:
anim_on_shake_value = strcmp(tuple->value->cstring, "1") == 0;
persist_write_bool(ANIM_ON_SHAKE, anim_on_shake_value);
return true;
}
return false;
}

