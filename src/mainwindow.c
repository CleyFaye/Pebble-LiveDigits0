/** @file
 * Main window for the watchface
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "config.h"
#include "digit_info.h"
#include "layout.h"
#include "widgetfilter.h"
#include "numberlayer.h"
#include "datelayer.h"

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

    /** Seconds widget */
    NumberLayer* widget_seconds;
    /** Seconds dot-ticker */
    InverterLayer* seconds_dot;
    /** Date widget */
    DateLayer* widget_date;

    /** Color invertion layer */
    InverterLayer* inverter;

    /** Animation timer */
    AppTimer* animation_timer;
    /** Timer to hide the widgets */
    AppTimer* widget_timer;

    /** Tick timer service registered */
    bool timer_service_registered;
    /** Tap service registered */
    bool tap_service_registered;

    /** Flag indicating that the current animations are forced (on shake, on
     * load)
     */
    bool extra_animation;
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

/** Register all pertinent services.
 *
 * This register different event handlers according to user configuration.
 */
static
void
main_window_register_services(window_info_t* info);

/** Unregister all services. */
static
void
main_window_unregister_services(window_info_t* info);

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

/** Switch animation speed to forced anim. speed.
 *
 * Forced anim speed last until there is no more animation to process.
 */
static
void
main_window_force_anim(window_info_t* info);

/** Set the visibility of widgets */
static
void
main_window_set_widget_visibility(window_info_t* info,
                                  bool visible);

/** Update the periodicity of the timer handler.
 *
 * This is used to switch from MINUTE_UNIT to SECOND_UNIT when the seconds are
 * displayed/hidden.
 */
static
void
main_window_update_timer_service(window_info_t* info);

/** Determine if widgets are currently visible.
 *
 * Widgets are visible if:
 * - They are always enabled
 * - They are enabled on shake, and the watch was shook recently
 */
static
bool
main_window_are_widgets_visible(window_info_t* info);

/** Return the window_info_t associated with a window */
static
window_info_t*
get_info(MainWindow* window);

/** Return the animation speed.
 *
 * @param forced Flag to retrieve the "forced" animation speed.
 */
static
animation_speed_t
cfgspeed_get(bool forced);

// EVENT HANDLERS =

/** Handle a time update callback */
static
void
handle_time_tick(struct tm* tick_time,
                 TimeUnits units_changed);

/** Handle a tap event */
static
void
handle_accel_tap(AccelAxisType axis,
                 int32_t direction);

/** Re-hide the widgets */
static
void
handle_widget_hidding(window_info_t* info);

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
    number_layer_set_quick_wrap(info->minutes,
                                quick_wrap);
    // Add them to the window
    layer_add_child(window_layer,
                    info->hours);
    layer_add_child(window_layer,
                    info->minutes);

    // Widgets
    // Seconds
    if (widget_is_active(WT_SECONDS)) {
        if (cfg_get_seconds_style() == SECONDS_STYLE_DOT) {
            GRect layer_rect;
            layer_rect.origin = layout_get_widget_offset(WT_SECONDS);
            layer_rect.size = GSize(seconds_dot_size,
                                    seconds_dot_size);
            info->seconds_dot = inverter_layer_create(layer_rect);
            layer_add_child(window_layer,
                            inverter_layer_get_layer(info->seconds_dot));
        } else {
            info->widget_seconds =
                number_layer_create(DS_SMALL,
                                    2,
                                    layout_get_widget_offset(WT_SECONDS));
            number_layer_set_animate_speed(info->widget_seconds,
                                           FAST_MERGED);
            number_layer_set_quick_wrap(info->widget_seconds,
                                        true);
            layer_add_child(window_layer,
                            info->widget_seconds);
        }
    }

    // Date
    info->widget_date = date_layer_create();
    if (info->widget_date) {
        layer_add_child(window_layer,
                        info->widget_date);
    }

    main_window_set_widget_visibility(info,
                                      !layout_widgets_hidden());

    // Must be last: the inverter, if required
    if (layout_is_white_background()) {
        info->inverter = inverter_layer_create(GRect(0,
                                               0,
                                               144,
                                               168));
        layer_add_child(window_layer,
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

    if (info->widget_seconds) {
        number_layer_destroy(info->widget_seconds);
        info->widget_seconds = NULL;
    }

    if (info->seconds_dot) {
        inverter_layer_destroy(info->seconds_dot);
        info->seconds_dot = NULL;
    }

    if (info->widget_date) {
        date_layer_destroy(info->widget_date);
        info->widget_date = NULL;
    }

    if (info->inverter) {
        inverter_layer_destroy(info->inverter);
        info->inverter = NULL;
    }
}

static
void
main_window_register_services(window_info_t* info)
{
    main_window_unregister_services(info);

    main_window_update_timer_service(info);

    if (cfg_get_anim_on_shake() ||
        layout_widgets_shaketimer() > 0) {
        accel_tap_service_subscribe(handle_accel_tap);
        info->tap_service_registered = true;
    }
}

static
void
main_window_unregister_services(window_info_t* info)
{
    if (info->timer_service_registered) {
        tick_timer_service_unsubscribe();
        info->timer_service_registered = false;
    }

    if (info->tap_service_registered) {
        accel_tap_service_unsubscribe();
    }
}

static
void
main_window_appear(Window* window)
{
    window_info_t* info = get_info(window);
    main_window_register_services(info);

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
    main_window_unregister_services(get_info(window));
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
    int minutes = tick_time->tm_min;
    int seconds = tick_time->tm_sec;

    if (clock_is_24h_style()) {
        hours = tick_time->tm_hour;
    } else {
        hours = (tick_time->tm_hour > 12)
                ? tick_time->tm_hour - 12
                : tick_time->tm_hour;
    }

    number_layer_set_number(info->hours,
                            hours,
                            animate);
    number_layer_set_number(info->minutes,
                            minutes,
                            animate);

    if (main_window_are_widgets_visible(info)) {

        if (info->widget_seconds) {
            bool animate_seconds =
                cfg_get_seconds_style() != SECONDS_STYLE_FIXED;

            if (animate_seconds) {
                int prev_second = seconds - 1;

                if (prev_second == -1) {
                    prev_second = 59;
                }

                number_layer_set_number(info->widget_seconds,
                                        prev_second,
                                        false);
            }

            number_layer_set_number(info->widget_seconds,
                                    seconds,
                                    animate_seconds);
        }

        if (info->seconds_dot) {
            layer_set_hidden(inverter_layer_get_layer(info->seconds_dot),
                             !layer_get_hidden(
                                 inverter_layer_get_layer(info->seconds_dot)));
        }

        if (info->widget_date) {
            date_layer_set_date(info->widget_date,
                                tick_time->tm_mday,
                                tick_time->tm_mon + 1);
        }
    }

    main_window_schedule_animation(info);

    if (seconds == 0) {
        switch (cfg_get_vibrate_every()) {
        case VIBRATE_EVERY_HOUR:
            if (minutes == 0) {
                vibes_double_pulse();
            }

            break;

        case VIBRATE_EVERY_MINUTE:
            vibes_short_pulse();
        }
    }
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
    main_window_force_anim(info);
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
void
main_window_force_anim(window_info_t* info)
{
    info->extra_animation = true;
    main_window_set_anim_speed(info,
                               cfgspeed_get(true));
    main_window_schedule_animation(info);
}

static
void
main_window_set_widget_visibility(window_info_t* info,
                                  bool visible)
{
    if (info->widget_seconds) {
        layer_set_hidden(info->widget_seconds,
                         !visible);
    }

    if (info->seconds_dot) {
        layer_set_hidden(inverter_layer_get_layer(info->seconds_dot),
                         !visible);
    }

    if (info->widget_date) {
        layer_set_hidden(info->widget_date,
                         !visible);
    }

    if (visible) {
        main_window_set_to_time(info,
                                false);
    }

    main_window_update_timer_service(info);
}

static
void
main_window_update_timer_service(window_info_t* info)
{
    bool seconds_visible = widget_is_active(WT_SECONDS) &&
                           main_window_are_widgets_visible(info);

    tick_timer_service_subscribe(seconds_visible
                                 ? SECOND_UNIT
                                 : MINUTE_UNIT,
                                 handle_time_tick);
    info->timer_service_registered = true;
}

static
bool
main_window_are_widgets_visible(window_info_t* info)
{
    switch (cfg_get_display_widgets()) {
    case DISPLAY_WIDGETS_NEVER:
        return false;

    case DISPLAY_WIDGETS_ALWAYS:
        return true;

    default:
        return info->widget_timer != NULL;
    }
}

static
window_info_t*
get_info(MainWindow* window)
{
    if (window == NULL) {
        window = global_main_window;
    }

    return (window_info_t*) window_get_user_data(window);
}

static
animation_speed_t
cfgspeed_get(bool forced)
{
    switch (forced
            ? cfg_get_anim_speed_forced()
            : cfg_get_anim_speed_normal()) {
    case ANIM_SPEED_NORMAL_SLOW_SEP:
        return SLOW_SEPARATE;

    case ANIM_SPEED_NORMAL_SLOW_MERGE:
        return SLOW_MERGED;

    case ANIM_SPEED_NORMAL_FAST_SEP:
        return FAST_SEPARATE;

    default:
    case ANIM_SPEED_NORMAL_FAST_MERGE:
        return FAST_MERGED;
    }
}

static
void
handle_time_tick(struct tm* tick_time,
                 TimeUnits units_changed)
{
    window_info_t* info = get_info(NULL);
    main_window_update_time(tick_time,
                            info,
                            cfg_get_anim_on_time());
}

static
void
handle_accel_tap(AccelAxisType axis,
                 int32_t direction)
{
    window_info_t* info = get_info(NULL);

    if (cfg_get_anim_on_shake()) {
        main_window_randomize_anim(info);
    }

    unsigned widget_timer = layout_widgets_shaketimer();

    if (widget_timer > 0) {
        if (info->widget_timer) {
            app_timer_reschedule(info->widget_timer,
                                 widget_timer * 1000);
        } else {
            info->widget_timer =
                app_timer_register(widget_timer * 1000,
                                   (AppTimerCallback) handle_widget_hidding,
                                   info);
        }

        main_window_set_widget_visibility(info,
                                          true);
    }
}

static
void
handle_widget_hidding(window_info_t* info)
{
    info->widget_timer = NULL;
    main_window_set_widget_visibility(info,
                                      false);
}

static
void
handle_anim_timer(window_info_t* info)
{
    bool need_animation = false;
    bool widget_need_animation = false;

    need_animation |= number_layer_animate(info->hours);
    need_animation |= number_layer_animate(info->minutes);

    if (info->widget_seconds) {
        widget_need_animation |= number_layer_animate(info->widget_seconds);
    }

    info->animation_timer = NULL;

    if (need_animation || widget_need_animation) {
        main_window_schedule_animation(info);
    }

    // Widget animation does not prevent extra_animation from being removed
    if (!need_animation) {
        if (info->extra_animation) {
            info->extra_animation = false;
            main_window_set_anim_speed(info,
                                       cfgspeed_get(false));
        }
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
    info->widget_seconds = NULL;
    info->seconds_dot = NULL;
    info->widget_date = NULL;
    info->inverter = NULL;
    info->animation_timer = NULL;
    info->widget_timer = NULL;
    info->timer_service_registered = false;
    info->tap_service_registered = false;
    info->extra_animation = false;
    window_set_user_data(result, info);
    global_main_window = result;
    main_window_schedule_animation(info);
    return result;
}

void
main_window_destroy(MainWindow* window)
{
    window_info_t* info = get_info(window);
    free(info);
    window_destroy(window);
}

void
main_window_update_settings(MainWindow* window)
{
    main_window_lay_widgets(window);
    window_info_t* info = get_info(window);
    main_window_set_to_time(info,
                            false);
    main_window_set_anim_speed(info,
                               cfgspeed_get(false));
    main_window_register_services(info);
    get_info(window)->extra_animation = false;
}

