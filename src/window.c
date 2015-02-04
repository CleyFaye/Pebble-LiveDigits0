#include <pebble.h>
#include "config.h"
#include "images.h"
#include "digitlayer.h"
#include "window.h"

static const int FAST_SPEED = 60;
    
typedef struct {
    DigitLayer* hour_tens;
    DigitLayer* hour_units;
    DigitLayer* minute_tens;
    DigitLayer* minute_units;
    InverterLayer* inverter;
    bool tap_registered;
} window_info_t;

static window_info_t* window_info_in_global_variable_just_because = NULL;
MainWindow* main_window_in_global = NULL;

static void main_window_load(Window* window);
static void main_window_appear(Window* window);
static void main_window_disappear(Window* window);
static void main_window_unload(Window* window);
static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed);
static void main_window_update_time(struct tm* tick_time, window_info_t* info, bool animate);
static void main_window_set_anim_speed(window_info_t* info, int anim_speed);
static void main_window_tap_handler(AccelAxisType axis, int32_t direction);
static void main_window_random_shake(window_info_t* info, struct tm* tick_time);
    
static void main_window_load(Window* window)
{
    window_set_background_color(window, GColorBlack);
    window_info_t* info = window_get_user_data(window);
    info->hour_tens = digit_layer_create(DIGIT_BIG, GPoint(8, 3), 2);
    info->hour_units = digit_layer_create(DIGIT_BIG, GPoint(61, 3), 9);
    info->minute_tens = digit_layer_create(DIGIT_SMALL, GPoint(50, 89), 5);
    info->minute_units = digit_layer_create(DIGIT_SMALL, GPoint(95, 89), 9);
    layer_add_child(window_get_root_layer(window), info->hour_tens);
    layer_add_child(window_get_root_layer(window), info->hour_units);
    layer_add_child(window_get_root_layer(window), info->minute_tens);
    layer_add_child(window_get_root_layer(window), info->minute_units);
}

static void main_window_appear(Window* window)
{
    tick_timer_service_subscribe(MINUTE_UNIT, handle_time_tick);
    window_info_t* info = window_get_user_data(window);
    time_t temp = time(NULL);
    struct tm* tick_time = localtime(&temp);
    main_window_update_config();
    if (config_get_animate_on_display()) {
        main_window_random_shake(info, tick_time);
    } else {
        main_window_update_time(tick_time, info, false);
    }
}

static void main_window_disappear(Window* window)
{
    tick_timer_service_unsubscribe();
    window_info_t* info = window_get_user_data(window);
    if (info->tap_registered) {
        accel_tap_service_unsubscribe();
        info->tap_registered = false;
    }
}

static void main_window_unload(Window* window)
{
}

static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed)
{
    // Ugh. At this point I have to rely on global variables :(
    window_info_t* info = window_info_in_global_variable_just_because;
    main_window_update_time(tick_time, info, true);
}

static void main_window_update_time(struct tm* tick_time, window_info_t* info, bool animate)
{
    int hours;
    int minutes;
    if (clock_is_24h_style()) {
        hours = tick_time->tm_hour;
    } else {
        hours = (tick_time->tm_hour > 12) ? tick_time->tm_hour - 12 : tick_time->tm_hour;
    }
    minutes = tick_time->tm_min;
    digit_layer_set_number(info->hour_tens, hours / 10, animate);
    digit_layer_set_number(info->hour_units, hours % 10, animate);
    digit_layer_set_number(info->minute_tens, minutes / 10, animate);
    digit_layer_set_number(info->minute_units, minutes % 10, animate);
    main_window_set_anim_speed(info, 0);
}

static void main_window_set_anim_speed(window_info_t* info, int anim_speed)
{
    if (info->hour_tens) {
        digit_layer_set_animate_speed(info->hour_tens, anim_speed);
        digit_layer_set_animate_speed(info->hour_units, anim_speed);
        digit_layer_set_animate_speed(info->minute_tens, anim_speed);
        digit_layer_set_animate_speed(info->minute_units, anim_speed);
    }
}

static void main_window_tap_handler(AccelAxisType axis, int32_t direction)
{
    time_t temp = time(NULL);
    struct tm* tick_time = localtime(&temp);
    main_window_random_shake(window_info_in_global_variable_just_because, tick_time);
}

static void main_window_random_shake(window_info_t* info, struct tm* tick_time)
{
    int hours = rand() % (clock_is_24h_style() ? 24 : 13);
    int minutes = rand() % 60;
    digit_layer_set_number(info->hour_tens, hours / 10, false);
    digit_layer_set_number(info->hour_units, hours % 10, false);
    digit_layer_set_number(info->minute_tens, minutes / 10, false);
    digit_layer_set_number(info->minute_units, minutes % 10, false);
    main_window_update_time(tick_time, info, true);
    main_window_set_anim_speed(info, FAST_SPEED);
}

MainWindow* main_window_create(void)
{
    Window* result = window_create();
    window_set_window_handlers(result, (WindowHandlers) {
        .load = main_window_load,
        .appear = main_window_appear,
        .disappear = main_window_disappear,
        .unload = main_window_unload
    });
    window_info_t* info = malloc(sizeof(window_info_t));
    info->hour_tens = NULL;
    info->hour_units = NULL;
    info->minute_tens = NULL;
    info->minute_units = NULL;
    info->inverter = NULL;
    window_set_user_data(result, info);
    window_info_in_global_variable_just_because = info;
    main_window_in_global = result;
    return result;
}

void main_window_destroy(MainWindow* window)
{
    window_info_t* info = window_get_user_data(window);
    if (info->hour_tens != NULL) {
        digit_layer_destroy(info->hour_tens);
        digit_layer_destroy(info->hour_units);
        digit_layer_destroy(info->minute_tens);
        digit_layer_destroy(info->minute_units);
    }
    if (info->inverter != NULL) {
        inverter_layer_destroy(info->inverter);
    }
    free(info);
    window_destroy(window);
}

void main_window_update_config(void)
{
    window_info_t* info = window_info_in_global_variable_just_because;
    MainWindow* window = main_window_in_global;
    bool quick_wrap = config_get_quick_wrap();
    digit_layer_set_quick_wrap(info->hour_tens, quick_wrap);
    digit_layer_set_quick_wrap(info->hour_units, quick_wrap);
    digit_layer_set_quick_wrap(info->minute_tens, quick_wrap);
    digit_layer_set_quick_wrap(info->minute_units, quick_wrap);
    if (config_get_white_background()) {
        if (!info->inverter) {
            info->inverter = inverter_layer_create(GRect(0, 0, 144, 168));
            layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(info->inverter));
        }
    } else {
        if (info->inverter) {
            inverter_layer_destroy(info->inverter);
            info->inverter = NULL;
        }
    }
    if (config_get_animate_on_shake()) {
        if (!info->tap_registered) {
            accel_tap_service_subscribe(main_window_tap_handler);
            info->tap_registered = true;
        }
    }
}
