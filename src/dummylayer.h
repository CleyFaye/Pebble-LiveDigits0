/** @file
 * A dummy placeholder for widgets
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DUMMYLAYER_H
#define INCL_DUMMYLAYER_H

// Layer
#include <pebble.h>

// =======
// TYPES =
// =======

/** A dummy layer displaying a number */
typedef Layer DummyLayer;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Create a dummy layer at given position.
 *
 * @param line The widget line, from 0 to 3 inclusive.
 */
DummyLayer*
dummy_layer_create(unsigned line);

/** Destroy a dummy layer. */
void
dummy_layer_destroy(DummyLayer* layer);

#endif

