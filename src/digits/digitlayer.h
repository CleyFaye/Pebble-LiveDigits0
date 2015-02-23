/** @file
 * Digit layer.
 *
 * Display a single digit that can be animated.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DIGITLAYER_H
#define INCL_DIGITLAYER_H

#include <pebble.h>

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
 * @param offset The offset of the top-left portion of the layer. (0,0) mean
 * that the digit will be stuck in the top-left corner of the window.
 * @return The layer instance
 */
DigitLayer*
digit_layer_create(digit_size_t size,
                   GPoint offset);

/** Set the number currently displayed by the layer.
 *
 * @param animate Set to true to animate from the currently displayed digit to
 * the requested number, false to immediately change it. If the layer is already
 * running an animation, it will not stop it. To do so call
 * digit_layer_kill_anim().
 */
void
digit_layer_set_number(DigitLayer* layer,
                       unsigned number,
                       bool animate);

/** Skip all animations step and immediately display the correct digit. */
void
digit_layer_kill_anim(DigitLayer* layer);

/** Set the quick-wrap flag.
 *
 * Quick wrap mean that instead of animating every digit between the currently
 * displayed digit and the required one, the layer will skip to 0 if a smaller
 * digit is required.
 *
 * For example, going from 2 to 5 will animate through 3 and 4, while going from
 * 5 to 2 will animate through 0 and 1 instead of 6, 7, 8, 9, 0, 1.
 */
void
digit_layer_set_quick_wrap(DigitLayer* layer,
                           bool quick_wrap);

/** Set the animation speed. */
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
 * @return true if the layer still need to be animated, false otherwise. A layer
 * returning false here will not need to be animated again unless its number is
 * changed.
 */
bool
digit_layer_animate(DigitLayer* layer);

/** Destroy a layer.
 */
void
digit_layer_destroy(DigitLayer* layer);

#endif

