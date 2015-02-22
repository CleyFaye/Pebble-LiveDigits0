/** @file
 * Manage digit images ressources.
 *
 * Digit images only contains a single segment at various orientation. Their
 * position and animation is handled elsewhere.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DIGIT_IMAGES_H
#define INCL_DIGIT_IMAGES_H

// GSize
// GBitmap
#include <pebble.h>
// digit_size_t
// segment_orientation_t
#include "digit_info.h"

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Load a set of segment images.
 *
 * Multiple call to this function for the same digit size will be handled
 * gracefully, and must be followed by the same number of unload calls.
 *
 * @param size The size of the segments to load.
 */
void
segment_load_images(digit_size_t size);

/** Unload a set of segment images.
 *
 * There must be as many calls to this function as to segment_load_images().
 *
 * @param size The size of the segments to unload.
 */
void
segment_unload_images(digit_size_t size);

/** Retrieve a segment image.
 *
 * Before calling this function an appropriate call to segment_load_images()
 * must have been done.
 *
 * @param size The size of the requested segment.
 * @param orientation The orientation of the segment.
 * @param bitmap_size A pointer to get the size of the bitmap. Can be NULL if
 * not required.
 */
GBitmap*
segment_get_image(digit_size_t size,
                           segment_orientation_t orientation,
                           GSize* bitmap_size);

#endif

