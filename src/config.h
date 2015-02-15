/* AUTO-GENERATED HEADER DO NOT MODIFY */
#ifndef __INCL_CONFIG_H
#define __INCL_CONFIG_H

#include <pebble.h>
    
#define ANIM_SPEED_FORCED_FAST_MERGE 3
#define ANIM_SPEED_FORCED_FAST_SEP 2
#define ANIM_SPEED_FORCED_SLOW_MERGE 1
#define ANIM_SPEED_FORCED_SLOW_SEP 0
#define ANIM_SPEED_NORMAL_FAST_MERGE 3
#define ANIM_SPEED_NORMAL_FAST_SEP 2
#define ANIM_SPEED_NORMAL_SLOW_MERGE 1
#define ANIM_SPEED_NORMAL_SLOW_SEP 0
#define BASE_LAYOUT_HOUR 0
#define BASE_LAYOUT_MIN 1
#define BATTERY_POSITION_1 1
#define BATTERY_POSITION_2 2
#define BATTERY_POSITION_3 3
#define BATTERY_POSITION_4 4
#define BATTERY_POSITION_OFF 0
#define BATTERY_STYLE_PERCENT 0
#define BATTERY_STYLE_PERCENT_ANIM 1
#define BATTERY_STYLE_STATE 2
#define BLUETOOTH_POSITION_1 1
#define BLUETOOTH_POSITION_2 2
#define BLUETOOTH_POSITION_3 3
#define BLUETOOTH_POSITION_4 4
#define BLUETOOTH_POSITION_OFF 0
#define BLUETOOTH_STYLE_ALWAYS 0
#define BLUETOOTH_STYLE_ONLYOFF 2
#define BLUETOOTH_STYLE_ONLYOFFBUZZ 3
#define BLUETOOTH_STYLE_ONLYOFFFORCE 4
#define BLUETOOTH_STYLE_ONLYOFFFORCEBUZZ 5
#define BLUETOOTH_STYLE_ONLYON 1
#define DATE_POSITION_1 1
#define DATE_POSITION_2 2
#define DATE_POSITION_3 3
#define DATE_POSITION_4 4
#define DATE_POSITION_OFF 0
#define DATE_STYLE_DAYFIRST 1
#define DATE_STYLE_DAYONLY 0
#define DATE_STYLE_MONFIRST 3
#define DATE_STYLE_MONONLY 2
#define DISPLAY_WIDGETS_ALWAYS 0
#define DISPLAY_WIDGETS_NEVER 3
#define DISPLAY_WIDGETS_SHAKE3 1
#define DISPLAY_WIDGETS_SHAKE6 2
#define HOUR_POSITION_LEFT 0
#define HOUR_POSITION_RIGHT 1
#define MINUTES_POSITION_LEFT 0
#define MINUTES_POSITION_RIGHT 1
#define SECONDS_POSITION_1 1
#define SECONDS_POSITION_2 2
#define SECONDS_POSITION_3 3
#define SECONDS_POSITION_4 4
#define SECONDS_POSITION_OFF 0
#define SECONDS_STYLE_ANIM 2
#define SECONDS_STYLE_DOT 0
#define SECONDS_STYLE_FIXED 1
#define VIBRATE_EVERY_HOUR 2
#define VIBRATE_EVERY_MINUTE 1
#define VIBRATE_EVERY_NO 0

typedef void(*config_callback_t)(void* data);

int cfg_get_hour_position(void);
int cfg_get_minutes_position(void);
int cfg_get_base_layout(void);
bool cfg_get_invert_colors(void);
int cfg_get_vibrate_every(void);
bool cfg_get_demo_mode(void);
bool cfg_get_skip_digits(void);
bool cfg_get_anim_on_time(void);
int cfg_get_anim_speed_normal(void);
bool cfg_get_anim_on_load(void);
bool cfg_get_anim_on_shake(void);
bool cfg_get_force_light(void);
int cfg_get_anim_speed_forced(void);
int cfg_get_display_widgets(void);
int cfg_get_date_position(void);
int cfg_get_date_style(void);
int cfg_get_seconds_position(void);
int cfg_get_seconds_style(void);
int cfg_get_bluetooth_position(void);
int cfg_get_bluetooth_style(void);
int cfg_get_battery_position(void);
int cfg_get_battery_style(void);

void cfg_init(config_callback_t callback, void* data);
void cfg_clear(void);

#endif
