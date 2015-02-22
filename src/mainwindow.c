/** @file
 * Main window for the watchface
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "digits/digit_info.h"
#include "digits/numberlayer.h"
#include "widgets/secondslayer.h"
#include "widgets/datelayer.h"
#include "config.h"
#include "layout.h"

#include "mainwindow.h"

// ===============
// PRIVATE TYPES =
// ===============

/** MainWindow state */
typedef struct {
    /** Hours number */
    NumberLayer* hours;
    /** Minutes number */
    NumberLayer* minutes;
    /** Seconds widget */
    SecondsLayer* widget_seconds;
    /** Date widget */
    DateLayer* widget_date;

    /** Color invertion layer */
    InverterLayer* inverter;

    /** Animation timer */
    AppTimer* animation_timer;
    /** Timer to hide the widgets */
    AppTimer* widget_timer;

    bool timer_service_registered;
    bool tap_service_registered;

    /** Flag indicating that the current animations are forced (on shake, on
     * load)
     */
    bool extra_animation;
} window_info_t;

// ===================
// PRIVATE VARIABLES =
// ===================

/** Global variable for window instance, used in some services callback */
static
MainWindow* global_main_window = NULL;

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

// COMPONENTS HANDLING =

/** Place components on the window.
 *
 * Create all the components layer according to config.
 */
static
void
lay_components(MainWindow* window);

/** Destroy all components. */
static
void
clear_components(window_info_t* info);

/** Set the animation speed of animated layers for hour/minutes */
static
void
set_anim_speed(window_info_t* info,
               bool forced_speed);

// SERVICES HANDLING =

/** Register all pertinent services.
 *
 * This register different event handlers according to user configuration.
 */
static
void
register_services(window_info_t* info);

/** Update the periodicity of the timer handler.
 *
 * This is used to switch from MINUTE_UNIT to SECOND_UNIT when the seconds are
 * displayed/hidden.
 */
static
void
register_timer_service(window_info_t* info);

/** Unregister all services. */
static
void
unregister_services(window_info_t* info);

// TIME UPDATES =

/** Set the time displayed to the current time */
static
void
set_to_current_time(window_info_t* info,
                    bool animate);

/** Update time displayed by the watchface.
 *
 * This also update widgets time.
 */
static
void
set_to_time(window_info_t* info,
            struct tm* tick_time,
            bool animate);

/** Update the time on widgets.
 *
 * This is called by main_window_update_time(), and when the widgets are
 * displayed.
 */
static
void
set_widget_to_time(window_info_t* info,
                   struct tm* tick_time);

// ANIMATION HANDLING =

/** Change all digits to random values to display animation. */
static
void
animate_random(window_info_t* info);

/** Schedule (if needed) the animation timer */
static
void
schedule_animation(window_info_t* info);

/** Switch animation speed to forced anim. speed.
 *
 * Forced anim speed last until there is no more animation to process.
 */
static
void
force_anim(window_info_t* info);

// WIDGETS MANAGEMENT =

/** Set the visibility of widgets */
static
void
set_widget_visibility(window_info_t* info,
                      bool visible);

/** Determine if widgets are currently visible.
 *
 * Widgets are visible if:
 * - They are always enabled
 * - They are enabled on shake, and the watch was shook recently
 */
static
bool
are_widgets_visible(window_info_t* info);

// EVENT HANDLERS =

/** Window loading function. Create the layers. */
static
void
handle_load(Window* window);

/** Callback for when the window is displayed */
static
void
handle_appear(Window* window);

/** Callback for when the window is hidden */
static
void
handle_disappear(Window* window);

/** Window unloading function. Clear the layers. */
static
void
handle_unload(Window* window);

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

// HELPER =

/** Return the window_info_t associated with a window */
static inline
window_info_t*
get_info(MainWindow* window)
{
    return (window_info_t*) window_get_user_data(window
            ? window
            : global_main_window);
}

/** Initialize the window state */
static
void
info_init(window_info_t* info);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

// COMPONENTS HANDLING =

static
void
lay_components(MainWindow* window)
{
    window_info_t* info = get_info(window);
    clear_components(info);

    Layer* window_layer = window_get_root_layer(window);
    // Base watchface (hours/minutes)
    info->hours = number_layer_create(DS_BIG,
                                      2,
                                      layout_get_hour_offset());
    info->minutes = number_layer_create(DS_MEDIUM,
                                        2,
                                        layout_get_minute_offset());
    bool quick_wrap = cfg_get_skip_digits();
    number_layer_set_quick_wrap(info->hours,
                                quick_wrap);
    number_layer_set_quick_wrap(info->minutes,
                                quick_wrap);
    set_anim_speed(info,
                   false);
    layer_add_child(window_layer,
                    info->hours);
    layer_add_child(window_layer,
                    info->minutes);

    // Widgets
#define WIDGETCREATE(type) \
    do { \
        info->widget_##type = type##_layer_create(); \
        if (info->widget_##type) { \
            layer_add_child(window_layer, \
                            info->widget_##type); \
        } \
    } while (false)
    WIDGETCREATE(seconds);
    WIDGETCREATE(date);
#undef WIDGETCREATE
    set_widget_visibility(info,
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
clear_components(window_info_t* info)
{
    if (info->hours) {
        number_layer_destroy(info->hours);
        info->hours = NULL;
        number_layer_destroy(info->minutes);
        info->minutes = NULL;
    }

#define WIDGETDESTROY(type) \
    do { \
        if (info->widget_##type) { \
            type##_layer_destroy(info->widget_##type); \
            info->widget_##type = NULL; \
        } \
    } while (false)
    WIDGETDESTROY(seconds);
    WIDGETDESTROY(date);
#undef WIDGETDESTROY

    if (info->inverter) {
        inverter_layer_destroy(info->inverter);
        info->inverter = NULL;
    }
}

static
void
set_anim_speed(window_info_t* info,
               bool forced_speed)
{
    animation_speed_t speed;

    switch (forced_speed
            ? cfg_get_anim_speed_forced()
            : cfg_get_anim_speed_normal()) {
    case ANIM_SPEED_NORMAL_SLOW_SEP:
        speed = SLOW_SEPARATE;
        break;

    case ANIM_SPEED_NORMAL_SLOW_MERGE:
        speed = SLOW_MERGED;
        break;

    case ANIM_SPEED_NORMAL_FAST_SEP:
        speed = FAST_SEPARATE;
        break;

    default:
    case ANIM_SPEED_NORMAL_FAST_MERGE:
        speed = FAST_MERGED;
    }

    if (info->hours) {
        number_layer_set_animate_speed(info->hours,
                                       speed);
        number_layer_set_animate_speed(info->minutes,
                                       speed);
    }
}

// SERVICES HANDLING =

static
void
register_services(window_info_t* info)
{
    unregister_services(info);

    register_timer_service(info);

    if (cfg_get_anim_on_shake() ||
        layout_widgets_shaketimer() > 0) {
        accel_tap_service_subscribe(handle_accel_tap);
        info->tap_service_registered = true;
    }
}

static
void
unregister_services(window_info_t* info)
{
    if (info->timer_service_registered) {
        tick_timer_service_unsubscribe();
        info->timer_service_registered = false;
    }

    if (info->tap_service_registered) {
        accel_tap_service_unsubscribe();
        info->tap_service_registered = false;
    }
}

static
void
register_timer_service(window_info_t* info)
{
    bool seconds_visible = layout_widget_is_active(WT_SECONDS) &&
                           are_widgets_visible(info);

    tick_timer_service_subscribe(seconds_visible
                                 ? SECOND_UNIT
                                 : MINUTE_UNIT,
                                 handle_time_tick);
    info->timer_service_registered = true;
}

// TIME UPDATES =

static
void
set_to_current_time(window_info_t* info,
                    bool animate)
{
    time_t temp = time(NULL);
    struct tm* tick_time = localtime(&temp);
    set_to_time(info,
                tick_time,
                animate);
}

static
void
set_to_time(window_info_t* info,
            struct tm* tick_time,
            bool animate)
{
    int hours = clock_is_24h_style()
                ? tick_time->tm_hour
                : ((tick_time->tm_hour > 12)
                   ? tick_time->tm_hour - 12
                   : tick_time->tm_hour);
    int minutes = tick_time->tm_min;
    int seconds = tick_time->tm_sec;

    number_layer_set_number(info->hours,
                            hours,
                            animate);
    number_layer_set_number(info->minutes,
                            minutes,
                            animate);

    if (are_widgets_visible(info)) {
        set_widget_to_time(info,
                           tick_time);
    }

    schedule_animation(info);

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
set_widget_to_time(window_info_t* info,
                   struct tm* tick_time)
{
    if (info->widget_seconds) {
        seconds_layer_set_time(info->widget_seconds,
                               tick_time->tm_sec);
    }

    if (info->widget_date) {
        date_layer_set_date(info->widget_date,
                            tick_time->tm_mday,
                            tick_time->tm_mon + 1);
    }
}

// ANIMATION HANDLING =

static
void
animate_random(window_info_t* info)
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
    number_layer_kill_anim(info->hours);
    number_layer_kill_anim(info->minutes);
    set_to_current_time(info,
                        true);
    force_anim(info);
}

static
void
schedule_animation(window_info_t* info)
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
force_anim(window_info_t* info)
{
    info->extra_animation = true;
    set_anim_speed(info,
                   true);
    schedule_animation(info);
}

// WIDGETS MANAGEMENT =

static
void
set_widget_visibility(window_info_t* info,
                      bool visible)
{
#define WIDGETVISIBILITY(type) \
    do { \
        if (info->widget_##type) { \
            layer_set_hidden(info->widget_##type, \
                             !visible); \
        } \
    } while (false)
    WIDGETVISIBILITY(seconds);
    WIDGETVISIBILITY(date);
#undef WIDGETVISIBILITY

    if (visible) {
        time_t temp = time(NULL);
        struct tm* tick_time = localtime(&temp);
        set_widget_to_time(info,
                           tick_time);
    }

    register_timer_service(info);
}

static
bool
are_widgets_visible(window_info_t* info)
{
    switch (cfg_get_display_widgets()) {
    case DISPLAY_WIDGETS_ALWAYS:
        return true;

    default:
    case DISPLAY_WIDGETS_NEVER:
        return info->widget_timer != NULL;
    }
}

static
void
handle_load(Window* window)
{
    window_set_background_color(window,
                                GColorBlack);
    lay_components(window);
}

static
void
handle_appear(Window* window)
{
    window_info_t* info = get_info(window);
    register_services(info);

    if (cfg_get_anim_on_load()) {
        animate_random(info);
    } else {
        set_to_current_time(info,
                            false);
    }
}

static
void
handle_disappear(Window* window)
{
    unregister_services(get_info(window));
}

static
void
handle_unload(Window* window)
{
    clear_components(get_info(window));
}

static
void
handle_time_tick(struct tm* tick_time,
                 TimeUnits units_changed)
{
    set_to_time(get_info(NULL),
                tick_time,
                cfg_get_anim_on_time());
}

static
void
handle_accel_tap(AccelAxisType axis,
                 int32_t direction)
{
    window_info_t* info = get_info(NULL);

    if (cfg_get_anim_on_shake()) {
        animate_random(info);
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

        set_widget_visibility(info,
                              true);
    }
}

static
void
handle_widget_hidding(window_info_t* info)
{
    info->widget_timer = NULL;
    set_widget_visibility(info,
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
        widget_need_animation |= seconds_layer_animate(info->widget_seconds);
    }

    info->animation_timer = NULL;

    if (need_animation || widget_need_animation) {
        schedule_animation(info);
    }

    // Widget animation does not prevent extra_animation from being removed
    if (!need_animation && info->extra_animation) {
        info->extra_animation = false;
        set_anim_speed(info,
                       false);
    }
}

// HELPER =

static
void
info_init(window_info_t* info)
{
    info->hours = NULL;
    info->minutes = NULL;
    info->widget_seconds = NULL;
    info->widget_date = NULL;
    info->inverter = NULL;
    info->animation_timer = NULL;
    info->widget_timer = NULL;
    info->timer_service_registered = false;
    info->tap_service_registered = false;
    info->extra_animation = false;
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

MainWindow*
main_window_create(void)
{
    Window* result = window_create();
    window_set_window_handlers(result, (WindowHandlers) {
        .load = handle_load,
         .appear = handle_appear,
          .disappear = handle_disappear,
           .unload = handle_unload
    });
    window_info_t* info = malloc(sizeof(window_info_t));
    info_init(info);
    window_set_user_data(result, info);
    global_main_window = result;
    schedule_animation(info);
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
    lay_components(window);
    window_info_t* info = get_info(window);
    set_to_current_time(info,
                        false);
    register_services(info);
}

