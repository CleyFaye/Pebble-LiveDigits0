/** @file
 * Manage digit images ressources.
 *
 * Digit images only contains a single segment at various orientation. Their
 * position and animation is handled in digitlayer and digit_anim.
 *
 * Loading and unloading keep a reference counter, so the corresponding
 * functions can be called multiple time without issues, as long as there is no
 * more calls to segment_unload_images() than to segment_load_images() for a
 * given size.
 * Each digit size is loaded independently.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DIGIT_IMAGES_H
#define INCL_DIGIT_IMAGES_H

#include <pebble.h>

#include "digit_info.h"

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Load a set of segment images.
 *
 * Multiple call to this function for the same digit size will be handled
 * gracefully, and must be followed by the same number of unload calls.
 */
void
segment_load_images(digit_size_t size);

/** Unload a set of segment images.
 *
 * There must be as many calls to this function as to segment_load_images().
 */
void
segment_unload_images(digit_size_t size);

/** Retrieve a segment image.
 *
 * Before calling this function an appropriate call to segment_load_images()
 * must have been done.
 *
 * @param bitmap_size A pointer to get the size of the bitmap. Can not be NULL
 */
GBitmap*
segment_get_image(digit_size_t size,
                  segment_orientation_t orientation,
                  GSize* bitmap_size);

#endif

