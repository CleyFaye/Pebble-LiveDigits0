/** @file
 * Manage the layers placement on the watchface.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_LAYOUT_H
#define INCL_LAYOUT_H

// GPoint
// GSize
#include <pebble.h>
// widget_type_t
#include "widgetfilter.h"

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

/** Return the offset for the hour number */
GPoint
layout_get_hour_offset(void);

/** Return the offset for the minute number */
GPoint
layout_get_minute_offset(void);

/** Return the offset for a widget.
 *
 * @param widget The widget type
 * @return The complete offset (position of the widget + widget offset)
 */
GPoint
layout_get_widget_offset(widget_type_t widget);

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
bool
layout_widgets_hidden(void);

/** Return the display delay for widgets on shake.
 *
 * @return The number of seconds the widgets must be displayed on shake, 0 if
 * they are always visible/hidden.
 */
int
layout_widgets_shaketimer(void);

#endif

