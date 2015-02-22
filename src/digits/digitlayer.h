/** @file
 * Digit layer.
 *
 * Display a single digit.
 *
 * This follows roughly the logic of the base Pebble SDK layers.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DIGITLAYER_H
#define INCL_DIGITLAYER_H

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

/** An animated digit layer. */
typedef Layer DigitLayer;

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

/** Create the digit layer.
 *
 * @param size The size of the digit.
 * @param offset The offset of the top-left portion of the layer. (0,0) mean
 * that the digit will be visible at the top-left corner of the window.
 * @return The layer instance
 */
DigitLayer*
digit_layer_create(digit_size_t size,
                   GPoint offset);

/** Set the number currently displayed by the layer.
 *
 * @param animate Set to true to animate from the currently displayed digit to
 * the requested number, false to immediately change it.
 */
void
digit_layer_set_number(DigitLayer* layer,
                       unsigned number,
                       bool animate);

/** Set the quick-wrap flag.
 *
 * Quick wrap mean that instead of animating every digit between the currently
 * displayed digit and the required one, the layer will skip to 0 if a smaller
 * digit is required.
 *
 * For example, going from 2 to 5 will animate through 3 and 4, while going from
 * 5 to 2 will animate through 0 and 1 instead of 6, 7, 8, 9, 0, 1.
 *
 * @param quick_wrap Set to true to use quick wrap (default to false)
 */
void
digit_layer_set_quick_wrap(DigitLayer* layer,
                           bool quick_wrap);

/** Set the animation speed.
 *
 * The animation speed indicate how fast the digit_layer_animate() will advance
 * through each animation step.
 */
void
digit_layer_set_animate_speed(DigitLayer* layer,
                              animation_speed_t speed);

/** Animate a layer.
 *
 * This function is responsible for actually moving the animation forward. Every
 * time it is called, and if the digit layer needs to be animated, it is done
 * there.
 * It is designed to be called every 100ms to ensure that FAST_MERGED animation
 * speed complete in less than a second.
 *
 * @return true if the layer still need to be animated, false otherwise.
 */
bool
digit_layer_animate(DigitLayer* layer);

/** Destroy a layer.
 *
 * Destroy a layer previously created with digit_layer_create().
 */
void
digit_layer_destroy(DigitLayer* layer);

#endif

