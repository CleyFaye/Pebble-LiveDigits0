#include <pebble.h>
#include "config.h"
#include "digit_info.h"
#include "numberlayer.h"
#include "window.h"

static const int FAST_SPEED = 60;

typedef struct {
    NumberLayer* hours;
    NumberLayer* minutes;
    InverterLayer* inverter;
    bool tap_registered;
    AppTimer* animation_timer;
    bool need_animation;
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
static void main_window_timer_callback(window_info_t* info);
static void main_window_schedule_animation(window_info_t* info);

static void main_window_load(Window* window)
{
    window_set_background_color(window, GColorBlack);
    window_info_t* info = window_get_user_data(window);
    info->hours = number_layer_create(DS_BIG,
                                      2,
                                      GPoint(8,
                                              3));
    info->minutes = number_layer_create(DS_MEDIUM,
                                        2,
                                        GPoint(50,
                                                89));
    layer_add_child(window_get_root_layer(window), info->hours);
    layer_add_child(window_get_root_layer(window), info->minutes);
}

static void main_window_appear(Window* window)
{
    tick_timer_service_subscribe(MINUTE_UNIT, handle_time_tick);
    window_info_t* info = window_get_user_data(window);
    time_t temp = time(NULL);
    struct tm* tick_time = localtime(&temp);
    main_window_update_config(window);

    if (cfg_get_anim_on_load()) {
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
    number_layer_set_number(info->hours,
                            hours,
                            animate);
    number_layer_set_number(info->minutes,
                            minutes,
                            animate);
    main_window_schedule_animation(info);
}

static void main_window_set_anim_speed(window_info_t* info, int anim_speed)
{
    if (info->hours) {
        number_layer_set_animate_speed(info->hours,
                                       anim_speed);
        number_layer_set_animate_speed(info->minutes,
                                       anim_speed);
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
    number_layer_set_number(info->hours,
                            hours,
                            false);
    number_layer_set_number(info->minutes,
                            minutes,
                            false);
    main_window_update_time(tick_time, info, true);
    main_window_set_anim_speed(info, FAST_MERGED);
}

static void main_window_timer_callback(window_info_t* info)
{
    bool need_animation = false;
    need_animation |= number_layer_animate(info->hours);
    need_animation |= number_layer_animate(info->minutes);

    info->animation_timer = NULL;

    if (need_animation) {
        main_window_schedule_animation(info);
    }
}

static void main_window_schedule_animation(window_info_t* info)
{
    if (!info->animation_timer) {
        info->animation_timer =
            app_timer_register(100,
                               (AppTimerCallback) main_window_timer_callback,
                               info);
    }
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
    info->hours = NULL;
    info->minutes = NULL;
    info->inverter = NULL;
    info->animation_timer = NULL;
    window_set_user_data(result, info);
    window_info_in_global_variable_just_because = info;
    main_window_in_global = result;
    main_window_schedule_animation(info);
    return result;
}

void main_window_destroy(MainWindow* window)
{
    window_info_t* info = window_get_user_data(window);

    if (info->hours != NULL) {
        number_layer_destroy(info->hours);
        number_layer_destroy(info->minutes);
    }

    if (info->inverter != NULL) {
        inverter_layer_destroy(info->inverter);
    }

    if (info->animation_timer) {
        app_timer_cancel(info->animation_timer);
        info->animation_timer = NULL;
    }

    free(info);
    window_destroy(window);
}

void main_window_update_config(void* data)
{
    MainWindow* window = (MainWindow*) data;
    window_info_t* info = window_get_user_data(window);
    bool quick_wrap = cfg_get_skip_digits();
    number_layer_set_quick_wrap(info->hours,
                                quick_wrap);
    number_layer_set_quick_wrap(info->minutes,
                                quick_wrap);

    if (cfg_get_invert_colors()) {
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

    if (cfg_get_anim_on_shake()) {
        if (!info->tap_registered) {
            accel_tap_service_subscribe(main_window_tap_handler);
            info->tap_registered = true;
        }
    }
}

