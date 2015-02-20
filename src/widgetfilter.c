/** @file
 * Filter widget presence
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

// bool
// false
#include <pebble.h>
// DISPLAY_WIDGETS_NEVER
// DATE_POSITION_OFF
// SECONDS_POSITION_OFF
// BATTERY_POSITION_OFF
// BLUETOOTH_POSITION_OFF
// cfg_get_display_widgets()
// cfg_get_bluetooth_position()
// cfg_get_battery_position()
// cfg_get_seconds_position()
// cfg_get_date_position()
#include "config.h"
// Associated header
#include "widgetfilter.h"

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

bool
widget_is_active(widget_type_t widget)
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

