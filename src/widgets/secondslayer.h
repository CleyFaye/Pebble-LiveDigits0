/** @file
 * Seconds digit
 *
 * Display the seconds.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_SECONDSLAYER_H
#define INCL_SECONDSLAYER_H

// Layer
#include <pebble.h>

// =======
// TYPES =
// =======

/** Seconds layer widget */
typedef Layer SecondsLayer;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Create the seconds layer.
 *
 * This take all settings from the configuration. If the seconds layer is not
 * enabled, return NULL.
 */
SecondsLayer*
seconds_layer_create(void);

/** Animate the layer (if required).
 *
 * @return true if more animation steps are needed, false otherwise.
 */
bool
seconds_layer_animate(SecondsLayer* layer);

/** Set the seconds counter */
void
seconds_layer_set_time(SecondsLayer* layer,
                       unsigned seconds);

/** Destroy a seconds layer. */
void
seconds_layer_destroy(SecondsLayer* layer);

#endif

