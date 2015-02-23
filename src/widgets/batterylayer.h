/** @file
 * Display the battery state.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_BATTERYLAYER_H
#define INCL_BATTERYLAYER_H

#include <pebble.h>

// =======
// TYPES =
// =======


typedef Layer BatteryLayer;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

BatteryLayer*
battery_layer_create(void);

void
battery_layer_set_state(BatteryLayer* layer,
                        bool charging,
                        uint8_t charge_percent);

inline
void
battery_layer_set_hidden(BatteryLayer* layer,
                         bool hidden)
{
    layer_set_hidden(layer,
                     hidden);
}

void
battery_layer_destroy(BatteryLayer* layer);

#endif

