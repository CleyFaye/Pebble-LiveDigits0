/** @file
 * Manage the layers placement on the watchface.
 *
 * These functions return the appropriate placement for various layer according
 * to the user configuration.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_LAYOUT_H
#define INCL_LAYOUT_H

#include <pebble.h>

#include "config.h"

#include "layout.h"

// =======
// TYPES =
// =======

typedef enum {
    WT_DATE,
    WT_SECONDS,
    WT_BATTERY,
    WT_BLUETOOTH,
    WIDGET_TYPE_COUNT
} widget_type_t;

// =========
// EXTERNS =
// =========

/** Size of a widget (they are all square) */
extern const unsigned widget_size;

/** Size of the seconds dot (it's a square) */
extern const unsigned seconds_dot_size;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Return the offset for the hour number layer */
GPoint
layout_get_hour_offset(void);

/** Return the offset for the minute number layer */
GPoint
layout_get_minute_offset(void);

/** Return the area for a widget. */
GRect
layout_get_widget_area(widget_type_t widget);

/** Determine if the colors must be inverted.
 *
 * When drawn, widgets are white on black background.
 *
 * This function take into account the watch color, and the user preference. If
 * their combination say that we want a white background, it returns true.
 */
bool
layout_is_white_background(void);

/** Return true if the widgets are hidden unless the watch is shook */
inline
bool
layout_widgets_hidden(void)
{
    return cfg_get_display_widgets() != DISPLAY_WIDGETS_ALWAYS;
}

/** Return the display delay for widgets on shake.
 *
 * @return The number of seconds the widgets must be displayed on shake, 0 if
 * they are always visible or always hidden.
 */
int
layout_widgets_shaketimer(void);

/** Determine if a given widget should be loaded.
 *
 * A widget should be loaded if it is enabled and if the general widget setting
 * is not to disable them all.
 */
bool
layout_widget_is_active(widget_type_t widget);

#endif

