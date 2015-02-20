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
// DummyLayer
// dummy_layer_create()
// dummy_layer_destroy()
#include "dummylayer.h"
// layout_get_hour_offset()
// layout_get_minute_offset()
#include "layout.h"
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

    // TEMP DUMMIES
    DummyLayer* dummies[4];

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
void
main_window_load(Window* window);

/** Place components on the window */
static
void
main_window_lay_widgets(MainWindow* window);

/** Remove all components */
static
void
main_window_remove_widgets(window_info_t* info);

/** Callback for when the window is displayed */
static
void
main_window_appear(Window* window);

/** Callback for when the window is hidden */
static
void
main_window_disappear(Window* window);

/** Window unloading function. Clear the layers. */
static
void
main_window_unload(Window* window);

/** Set the time displayed correctly. */
static
void
main_window_set_to_time(window_info_t* info,
                        bool animate);

/** Update time displayed by the window */
static
void
main_window_update_time(struct tm* tick_time,
                        window_info_t* info,
                        bool animate);

/** Set the animation speed of animated layers */
static
void
main_window_set_anim_speed(window_info_t* info,
                           animation_speed_t speed);

/** Change all digits to random values to display animation. */
static
void
main_window_randomize_anim(window_info_t* info);

/** Schedule (if needed) the animation timer */
static
void
main_window_schedule_animation(window_info_t* info);

/** Return the window_info_t associated with a window */
static
window_info_t*
get_info(MainWindow* window);

// EVENT HANDLERS =

/** Handle a time update callback */
static
void
handle_time_tick(struct tm* tick_time,
                 TimeUnits units_changed);

/** Handle the animation timer event */
static
void
handle_anim_timer(window_info_t* info);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void
main_window_load(Window* window)
{
    window_set_background_color(window,
                                GColorBlack);
    main_window_lay_widgets(window);
}

static
void
main_window_lay_widgets(MainWindow* window)
{
    window_info_t* info = get_info(window);
    main_window_remove_widgets(info);
    Layer* window_layer = window_get_root_layer(window);
    // Base watchface (hours/minutes)
    // Create them
    info->hours = number_layer_create(DS_BIG,
                                      2,
                                      layout_get_hour_offset());
    info->minutes = number_layer_create(DS_MEDIUM,
                                        2,
                                        layout_get_minute_offset());
    // Set them
    bool quick_wrap = cfg_get_skip_digits();
    number_layer_set_quick_wrap(info->hours,
                                quick_wrap);
    number_layer_set_quick_wrap(info->hours,
                                quick_wrap);
    // Add them to the window
    layer_add_child(window_layer,
                    info->hours);
    layer_add_child(window_layer,
                    info->minutes);

    for (unsigned i = 0;
         i < 4;
         ++i) {
        info->dummies[i] = dummy_layer_create(i);
        layer_add_child(window_layer,
                        info->dummies[i]);
    }

    // Must be last: the inverter, if required
    if (layout_is_white_background()) {
        info->inverter = inverter_layer_create(GRect(0,
                                               0,
                                               144,
                                               168));
        layer_add_child(window_get_root_layer(window),
                        inverter_layer_get_layer(info->inverter));
    }
}

static
void
main_window_remove_widgets(window_info_t* info)
{
    if (info->hours) {
        number_layer_destroy(info->hours);
        info->hours = NULL;
        number_layer_destroy(info->minutes);
        info->minutes = NULL;
    }

    if (info->dummies[0]) {
        for (unsigned i = 0;
             i < 4;
             ++i) {
            dummy_layer_destroy(info->dummies[i]);
        }
    }

    if (info->inverter) {
        inverter_layer_destroy(info->inverter);
        info->inverter = NULL;
    }
}

static
void
main_window_appear(Window* window)
{
    tick_timer_service_subscribe(MINUTE_UNIT,
                                 handle_time_tick);

    window_info_t* info = get_info(window);

    if (cfg_get_anim_on_load()) {
        main_window_randomize_anim(info);
    } else {
        main_window_set_to_time(info,
                                false);
    }
}

static
void
main_window_disappear(Window* window)
{
    tick_timer_service_unsubscribe();
}

static
void
main_window_unload(Window* window)
{
    main_window_remove_widgets(window_get_user_data(window));
}

static
void
main_window_set_to_time(window_info_t* info,
                        bool animate)
{
    time_t temp = time(NULL);
    struct tm* tick_time = localtime(&temp);
    main_window_update_time(tick_time,
                            info,
                            animate);
}

static
void
main_window_update_time(struct tm* tick_time,
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
void
main_window_set_anim_speed(window_info_t* info,
                           animation_speed_t speed)
{
    if (info->hours) {
        number_layer_set_animate_speed(info->hours,
                                       speed);
        number_layer_set_animate_speed(info->minutes,
                                       speed);
    }
}

static
void
main_window_randomize_anim(window_info_t* info)
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
    main_window_set_to_time(info,
                            true);
}

static
void
main_window_schedule_animation(window_info_t* info)
{
    if (!info->animation_timer) {
        info->animation_timer =
            app_timer_register(100,
                               (AppTimerCallback) handle_anim_timer,
                               info);
    }
}

static
window_info_t*
get_info(MainWindow* window)
{
    return (window_info_t*) window_get_user_data(window);
}

static
void
handle_time_tick(struct tm* tick_time,
                 TimeUnits units_changed)
{
    window_info_t* info = get_info(global_main_window);
    main_window_update_time(tick_time,
                            info,
                            true);
}

static
void
handle_anim_timer(window_info_t* info)
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
    window_info_t* info = get_info(window);
    free(info);
    window_destroy(window);
}

void
main_window_update_settings(MainWindow* window)
{
    main_window_lay_widgets(window);
    main_window_set_to_time(get_info(window),
                            false);
}

