/** @file
 * Filter widget presence
 *
 * A widget is present only if widgets are not disabled, and the specific widget
 * is enabled.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.ms file.
 */

#ifndef INCL_WIDGETFILTER_H
#define INCL_WIDGETFILTER_H

// bool
#include <pebble.h>

// =======
// TYPES =
// =======

typedef enum {
    WT_DATE,
    WT_SECONDS,
    WT_BATTERY,
    WT_BLUETOOTH
} widget_type_t;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Determine if a given widget should be loaded */
bool
widget_is_active(widget_type_t widget);

#endif

