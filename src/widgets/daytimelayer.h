/** @file
 * Display a little "AM" or "TM" when required.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DAYTIMELAYER_H
#define INCL_DAYTIMELAYER_H

#include <pebble.h>

// =======
// TYPES =
// =======

typedef Layer DayTimeLayer;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

DayTimeLayer*
daytime_layer_create(void);

void
daytime_layer_set_time(DayTimeLayer* layer,
        struct tm* tick_time);

void
daytime_layer_destroy(DayTimeLayer* layer);

#endif

