/** @file
 * Manage the layers placement on the watchface.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "config.h"

#include "layout.h"

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

GRect
layout_get_widget_area(widget_type_t widget)
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
        return GRect(0, 0,
                     widget_size, widget_size);
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
        6, 46, 90, 129
    };
    static const unsigned widget_y_offset_hour_bottom[4] = {
        3, 42, 85, 125
    };
    GRect result;
    bool aligned_with_hour;

    if (cfg_get_base_layout() == BASE_LAYOUT_HOUR) {
        result.origin.y = widget_y_offset_hour_top[line];
        aligned_with_hour = line <= 1;
    } else {
        result.origin.y = widget_y_offset_hour_bottom[line];
        aligned_with_hour = line >= 2;
    }

    result.origin.x = aligned_with_hour
                      ? ((cfg_get_hour_position() == HOUR_POSITION_LEFT)
                         ? widget_x_offset_hour_left
                         : widget_x_offset_hour_right)
                      : ((cfg_get_minutes_position() == MINUTES_POSITION_LEFT)
                         ? widget_x_offset_minutes_left
                         : widget_x_offset_minutes_right);
    result.size = GSize(widget_size,
                        widget_size);
    return result;
}

bool
layout_is_white_background(void)
{
    bool need_white_background;

    switch (watch_info_get_color()) {
    // TODO this will disappear to support colors
    default:
        need_white_background = false;
        break;
    }

    return cfg_get_invert_colors()
           ? !need_white_background
           : need_white_background;
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

bool
layout_widget_is_active(widget_type_t widget)
{
    if (cfg_get_display_widgets() == DISPLAY_WIDGETS_NEVER) {
        return false;
    }

    switch (widget) {
    case WT_DATE:
        return cfg_get_date_position() != DATE_POSITION_OFF;

    case WT_SECONDS:
        return cfg_get_seconds_position() != SECONDS_POSITION_OFF;

    case WT_BATTERY:
        return cfg_get_battery_position() != BATTERY_POSITION_OFF;

    case WT_BLUETOOTH:
        return cfg_get_bluetooth_position() != BLUETOOTH_POSITION_OFF;

    default:
        return false;
    }
}

