/** @file
 * Main window for the watchface
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

// bool
// InverterLayer
// AppTimer
// struct Window
// struct tm
// TimeUnits
// int32_t
// AccelAxisType
// AppTimerCallback
// NULL
// localtime()
// rand()
// malloc()
// free()
// inverter_layer_create()
// inverter_layer_get_layer()
// inverter_layer_destroy()
// window_set_background_color()
// window_get_user_data()
// window_get_root_layer()
// window_set_user_data()
// window_create()
// window_set_window_handlers()
// window_destroy()
// layer_add_child()
// tick_timer_service_subscribe()
// tick_timer_service_unsubscribe()
// clock_is_24h_style()
// app_timer_register()
// app_timer_cancel()
#include <pebble.h>
// cfg_get_anim_on_load()
// cfg_get_invert_colors()
#include "config.h"
// animation_speed_t
#include "digit_info.h"
// NumberLayer
// number_layer_create()
// number_layer_set_number()
// number_layer_set_animate_speed()
// number_layer_animate()
// number_layer_set_quick_wrap()
// number_layer_destroy()
#include "numberlayer.h"
// Associated header
#include "mainwindow.h"

// ===============
// PRIVATE TYPES =
// ===============

/** MainWindow state */
typedef struct {
    // LAYERS =
    /** Hours number */
    NumberLayer* hours;
    /** Minutes number */
    NumberLayer* minutes;
    /** Color invertion layer */
    InverterLayer* inverter;

    /** Animation timer */
    AppTimer* animation_timer;
} window_info_t;

// ===================
// PRIVATE VARIABLES =
// ===================

/** Global variable for window instance */
static
MainWindow* global_main_window = NULL;

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

/** Window loading function. Create the layers. */
static
void main_window_load(Window* window);

/** Callback for when the window is displayed */
static
void main_window_appear(Window* window);

/** Callback for when the window is hidden */
static
void main_window_disappear(Window* window);

/** Window unloading function. Clear the layers. */
static
void main_window_unload(Window* window);

/** Update time displayed by the window */
static
void main_window_update_time(struct tm* tick_time,
                             window_info_t* info,
                             bool animate);

/** Set the animation speed of animated layers */
static
void main_window_set_anim_speed(window_info_t* info,
                                animation_speed_t speed);

/** Change all digits to random values to display animation. */
static
void main_window_randomize_anim(window_info_t* info,
                                struct tm* tick_time);

/** Schedule (if needed) the animation timer */
static
void main_window_schedule_animation(window_info_t* info);

// EVENT HANDLERS =

/** Handle a time update callback */
static
void handle_time_tick(struct tm* tick_time,
                      TimeUnits units_changed);

/** Handle the animation timer event */
static
void handle_anim_timer(window_info_t* info);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void main_window_load(Window* window)
{
    window_set_background_color(window,
                                GColorBlack);
    window_info_t* info = window_get_user_data(window);
    info->hours = number_layer_create(DS_BIG,
                                      2,
                                      GPoint(5,
                                              3));
    info->minutes = number_layer_create(DS_MEDIUM,
                                        2,
                                        GPoint(50,
                                                89));
    layer_add_child(window_get_root_layer(window),
                    info->hours);
    layer_add_child(window_get_root_layer(window),
                    info->minutes);
}

static
void main_window_appear(Window* window)
{
    tick_timer_service_subscribe(MINUTE_UNIT,
                                 handle_time_tick);
    window_info_t* info = window_get_user_data(window);
    time_t temp = time(NULL);
    struct tm* tick_time = localtime(&temp);
    main_window_update_settings(window);

    if (cfg_get_anim_on_load()) {
        main_window_randomize_anim(info,
                                   tick_time);
    } else {
        main_window_update_time(tick_time,
                                info,
                                false);
    }
}

static
void main_window_disappear(Window* window)
{
    tick_timer_service_unsubscribe();
}

static
void main_window_unload(Window* window)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "UNLOAD");
}

static
void main_window_update_time(struct tm* tick_time,
                             window_info_t* info,
                             bool animate)
{
    int hours;
    int minutes;

    if (clock_is_24h_style()) {
        hours = tick_time->tm_hour;
    } else {
        hours = (tick_time->tm_hour > 12)
                ? tick_time->tm_hour - 12
                : tick_time->tm_hour;
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

static
void main_window_set_anim_speed(window_info_t* info,
                                animation_speed_t speed)
{
    if (info->hours) {
        number_layer_set_animate_speed(info->hours,
                                       speed);
        number_layer_set_animate_speed(info->minutes,
                                       speed);
    }
}

static void main_window_randomize_anim(window_info_t* info,
                                       struct tm* tick_time)
{
    int hours = rand() %
                (clock_is_24h_style()
                 ? 24
                 : 13);
    int minutes = rand() % 60;
    number_layer_set_number(info->hours,
                            hours,
                            false);
    number_layer_set_number(info->minutes,
                            minutes,
                            false);
    main_window_update_time(tick_time,
                            info,
                            true);
}

static
void main_window_schedule_animation(window_info_t* info)
{
    if (!info->animation_timer) {
        info->animation_timer =
            app_timer_register(100,
                               (AppTimerCallback) handle_anim_timer,
                               info);
    }
}

static
void handle_time_tick(struct tm* tick_time,
                      TimeUnits units_changed)
{
    window_info_t* info = window_get_user_data(global_main_window);
    main_window_update_time(tick_time,
                            info,
                            true);
}

static
void handle_anim_timer(window_info_t* info)
{
    bool need_animation = false;
    need_animation |= number_layer_animate(info->hours);
    need_animation |= number_layer_animate(info->minutes);

    info->animation_timer = NULL;

    if (need_animation) {
        main_window_schedule_animation(info);
    }
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

MainWindow*
main_window_create(void)
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
    global_main_window = result;
    main_window_schedule_animation(info);
    return result;
}

void
main_window_destroy(MainWindow* window)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "DESTROY");
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

void
main_window_update_settings(MainWindow* window)
{
    window_info_t* info = window_get_user_data(window);
    bool quick_wrap = cfg_get_skip_digits();
    number_layer_set_quick_wrap(info->hours,
                                quick_wrap);
    number_layer_set_quick_wrap(info->minutes,
                                quick_wrap);

    if (cfg_get_invert_colors()) {
        if (!info->inverter) {
            info->inverter = inverter_layer_create(GRect(0,
                                                   0,
                                                   144,
                                                   168));
            layer_add_child(window_get_root_layer(window),
                            inverter_layer_get_layer(info->inverter));
        }
    } else {
        if (info->inverter) {
            inverter_layer_destroy(info->inverter);
            info->inverter = NULL;
        }
    }
}

