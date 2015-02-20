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
// cfg_get_hour_position()
// cfg_get_minutes_position()
// cfg_get_base_layout()
#include "config.h"
// Associated header
#include "layout.h"

// =========
// EXTERNS =
// =========

const unsigned widget_size = 36;

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
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Minute offset:%i,%i", result.x, result.y);
    return result;
}

GPoint
layout_get_widget_offset(unsigned line,
                         GPoint widget_offset)
{
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

