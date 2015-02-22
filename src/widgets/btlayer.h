/** @file
 * Display the state of the bluetooth connection.
 *
 * For convenience the widget also take care of vibrating the watch if needed.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_BTLAYER_H
#define INCL_BTLAYER_H

#include <pebble.h>

// =======
// TYPES =
// =======

/** A BtLayer */
typedef Layer BtLayer;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Create the Bt layer.
 *
 * This take settings from config.
 */
BtLayer*
bt_layer_create(void);

/** Change the displayed state of the Bluetooth. */
void
bt_layer_set_state(BtLayer* layer,
                   bool connected);

/** Request to hide/show the widget.
 *
 * The Bluetooth widget might stay visible if the configuration say so. */
void
bt_layer_set_hidden(BtLayer* layer,
                    bool hidden);

void
bt_layer_destroy(BtLayer* layer);

#endif

