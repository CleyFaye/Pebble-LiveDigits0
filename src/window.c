#include <pebble.h>
#include "images.h"
#include "digitlayer.h"
#include "window.h"

typedef struct {
    DigitLayer* hour_tens;
    DigitLayer* hour_units;
    DigitLayer* minute_tens;
    DigitLayer* minute_units;
} window_info_t;

static window_info_t* window_info_in_global_variable_just_because_of_tick_timer_service = NULL;

static void main_window_load(Window* window);
static void main_window_appear(Window* window);
static void main_window_disappear(Window* window);
static void main_window_unload(Window* window);
static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed);
static void main_window_update_time(struct tm* tick_time, window_info_t* info, bool animate);
    
static void main_window_load(Window* window)
{
    window_set_background_color(window, GColorBlack);
    window_info_t* info = window_get_user_data(window);
    info->hour_tens = digit_layer_create(DIGIT_BIG, GPoint(8, 3));
    info->hour_units = digit_layer_create(DIGIT_BIG, GPoint(61, 3));
    info->minute_tens = digit_layer_create(DIGIT_SMALL, GPoint(50, 89));
    info->minute_units = digit_layer_create(DIGIT_SMALL, GPoint(95, 89));
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
    main_window_update_time(tick_time, info, false);
}

static void main_window_disappear(Window* window)
{
    tick_timer_service_unsubscribe();
}

static void main_window_unload(Window* window)
{
}

static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed)
{
    // Ugh. At this point I have to rely on global variables :(
    window_info_t* info = window_info_in_global_variable_just_because_of_tick_timer_service;
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
    window_set_user_data(result, info);
    window_info_in_global_variable_just_because_of_tick_timer_service = info;
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
    free(info);
    window_destroy(window);
}
    