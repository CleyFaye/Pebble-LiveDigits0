/** @file
 * Manage the layers placement on the watchface.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

// GPoint
// watch_info_get_color()
#include <pebble.h>
// HOUR_POSITION_LEFT
// MINUTES_POSITION_LEFT
// BASE_LAYOUT_HOUR
// DISPLAY_WIDGETS_ALWAYS
// SECONDS_STYLE_DOT
// cfg_get_hour_position()
// cfg_get_minutes_position()
// cfg_get_base_layout()
// cfg_get_date_position()
// cfg_get_seconds_position()
// cfg_get_battery_position()
// cfg_get_bluetooth_position()
// cfg_get_display_widgets()
// cfg_get_seconds_style()
#include "config.h"
// widget_type_t
#include "widgetfilter.h"
// Associated header
#include "layout.h"

// ================
// PRIVATE CONSTS =
// ================

static
GPoint widget_offset[WIDGET_TYPE_COUNT] = {
    // WT_DATE
    { .x = 0, .y = 0},
    // WT_SECONDS
    { .x = 1, .y = 2},
    // WT_BATTERY
    { .x = 0, .y = 0},
    // WT_BLUETOOTH
    { .x = 0, .y = 0}
};

// =========
// EXTERNS =
// =========

const unsigned widget_size = 36;
const unsigned seconds_dot_size = 4;

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

GPoint
layout_get_hour_offset(void)
{
    static const unsigned hour_number_left_align = 5;
    static const unsigned hour_number_right_align = 42;
    static const unsigned hour_number_top_align = 3;
    static const unsigned hour_number_bottom_align = 82;

    GPoint result;
    result.x =
        (cfg_get_hour_position() == HOUR_POSITION_LEFT)
        ? hour_number_left_align
        : hour_number_right_align;
    result.y =
        (cfg_get_base_layout() == BASE_LAYOUT_HOUR)
        ? hour_number_top_align
        : hour_number_bottom_align;
    return result;
}

GPoint
layout_get_minute_offset(void)
{
    static const unsigned minute_number_left_align = 7;
    static const unsigned minute_number_right_align = 50;
    static const unsigned minute_number_top_align = 2;
    static const unsigned minute_number_bottom_align = 89;

    GPoint result;
    result.x =
        (cfg_get_minutes_position() == MINUTES_POSITION_LEFT)
        ? minute_number_left_align
        : minute_number_right_align;
    result.y =
        (cfg_get_base_layout() == BASE_LAYOUT_HOUR)
        ? minute_number_bottom_align
        : minute_number_top_align;
    return result;
}

GPoint
layout_get_widget_offset(widget_type_t widget)
{
    unsigned line;

    switch (widget) {
    default:
    case WT_DATE:
        line = cfg_get_date_position();
        break;

    case WT_SECONDS:
        line = cfg_get_seconds_position();
        break;

    case WT_BATTERY:
        line = cfg_get_battery_position();
        break;

    case WT_BLUETOOTH:
        line = cfg_get_bluetooth_position();
        break;
    }

    if (line == 0) {
        // Woops, this widget is not enabled
        return GPointZero;
    }

    // Line indices start at 0, config values start at 1
    --line;
    // X offset for widgets aligned with hour/minutes
    static const unsigned widget_x_offset_hour_left = 105;
    static const unsigned widget_x_offset_hour_right = 3;
    static const unsigned widget_x_offset_minutes_left = 101;
    static const unsigned widget_x_offset_minutes_right = 7;
    // Y offset when hours are on top/bottom (for each lines)
    static const unsigned widget_y_offset_hour_top[4] = {
        6,
        46,
        90,
        129
    };
    static const unsigned widget_y_offset_hour_bottom[4] = {
        3,
        42,
        85,
        125
    };
    GPoint result;
    bool aligned_with_hour;

    if (cfg_get_base_layout() == BASE_LAYOUT_HOUR) {
        result.y = widget_y_offset_hour_top[line];
        aligned_with_hour = line <= 1;
    } else {
        result.y = widget_y_offset_hour_bottom[line];
        aligned_with_hour = line >= 2;
    }

    result.x = aligned_with_hour
               ? ((cfg_get_hour_position() == HOUR_POSITION_LEFT)
                  ? widget_x_offset_hour_left
                  : widget_x_offset_hour_right)
               : ((cfg_get_minutes_position() == MINUTES_POSITION_LEFT)
                  ? widget_x_offset_minutes_left
                  : widget_x_offset_minutes_right);

    // Exception
    if (widget == WT_SECONDS &&
        cfg_get_seconds_style() == SECONDS_STYLE_DOT) {
        // Second dot is centered on widget size
        result.x += widget_size / 2 - seconds_dot_size / 2;
        result.y += widget_size / 2 - seconds_dot_size / 2;
    } else {
        result.x += widget_offset[widget].x;
        result.y += widget_offset[widget].y;
    }

    return result;
}

bool
layout_is_white_background(void)
{
    bool need_white_background;

    switch (watch_info_get_color()) {
    default:
    case WATCH_INFO_COLOR_UNKNOWN:
    case WATCH_INFO_COLOR_BLACK:
    case WATCH_INFO_COLOR_GREY:
    case WATCH_INFO_COLOR_STAINLESS_STEEL:
    case WATCH_INFO_COLOR_MATTE_BLACK:
        need_white_background = false;
        break;

    case WATCH_INFO_COLOR_WHITE:
    case WATCH_INFO_COLOR_RED:
    case WATCH_INFO_COLOR_ORANGE:
    case WATCH_INFO_COLOR_BLUE:
    case WATCH_INFO_COLOR_GREEN:
    case WATCH_INFO_COLOR_PINK:
        need_white_background = true;
    }

    return cfg_get_invert_colors()
           ? !need_white_background
           : need_white_background;
}

bool
layout_widgets_hidden(void)
{
    int cfg_value = cfg_get_display_widgets();
    return cfg_value != DISPLAY_WIDGETS_ALWAYS;
}

int
layout_widgets_shaketimer(void)
{
    switch (cfg_get_display_widgets()) {
    default:
    case DISPLAY_WIDGETS_NEVER:
    case DISPLAY_WIDGETS_ALWAYS:
        return 0;

    case DISPLAY_WIDGETS_SHAKE3:
        return 3;

    case DISPLAY_WIDGETS_SHAKE6:
        return 6;
    }
}

