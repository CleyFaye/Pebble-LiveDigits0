/** @file
 * Date digit
 *
 * Display the date according to the user config.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DATELAYER_H
#define INCL_DATELAYER_H

#include <pebble.h>

// =======
// TYPES =
// =======

/** Date layer widget */
typedef Layer DateLayer;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Create the date layer.
 *
 * This take all settings from the configuration. If the date layer is not
 * enabled, return NULL.
 */
DateLayer*
date_layer_create(void);

/** Set the date */
void
date_layer_set_date(DateLayer* layer,
                    unsigned day,
                    unsigned month);

void
date_layer_set_hidden(DateLayer* layer,
                      bool hidden);

/** Destroy a date layer. */
void
date_layer_destroy(DateLayer* layer);

#endif

