/** @file
 * Number layer.
 *
 * A number layer is made of multiple digit layers, and update them accordingly.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_NUMBERLAYER_H
#define INCL_NUMBERLAYER_H

// GPoint
// Layer
// bool
#include <pebble.h>
// digit_size_t
// animation_speed_t
#include "digit_info.h"

// =======
// TYPES =
// =======

/** A number layer. */
typedef Layer NumberLayer;

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

/** Create a number layer.
 *
 * @param digit_count The number of digit in this number.
 */
NumberLayer*
number_layer_create(digit_size_t size,
                    unsigned digit_count,
                    GPoint offset);

/** Set the number displayed by the layer.
 *
 * @param animate Set to true to animate from the currently displayed digit to
 * the requested number, false to immediately change it.
 */
void
number_layer_set_number(NumberLayer* layer,
                        unsigned number,
                        bool animate);

/** Set the quick-wrap falg.
 *
 * If enabled, number will not iterate over every number, but shortcut to 0 if
 * it is faster. See digit_layer_set_quick_wrap() for more details.
 */
void
number_layer_set_quick_wrap(NumberLayer* layer,
                            bool quick_wrap);

/** Set the animation speed.
 *
 * The animation speed indicate how fast the number changes.
 */
void
number_layer_set_animate_speed(NumberLayer* layer,
                               animation_speed_t speed);

/** Animate a layer.
 *
 * This function is responsible for actually moving the animation forward. It
 * must be called every 100ms.
 *
 * @return true if the layer still need to be animated, false otherwise.
 */
bool
number_layer_animate(NumberLayer* layer);

/** Destroy a layer.
 *
 * Destroy a layer previously created with number_layer_destroy().
 */
void
number_layer_destroy(NumberLayer* layer);

#endif

