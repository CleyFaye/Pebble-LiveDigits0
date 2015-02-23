/** @file
 * Utility code
 *
 * Generic code.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_UTILS_H
#define INCL_UTILS_H

#include <pebble.h>

// =======
// TYPES =
// =======

/** Layer custom-data initialization function */
typedef void(*layer_data_init_t)(void* data);

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Load a list of resource identifier into an array of bitmaps.
 *
 * The target array must already be initialized.
 *
 * @param resource_ids A list of resource id to load
 * @param target_array The bitmap array to fill
 * @param resources_count The number of resources to load
 */
void
load_bitmap_into_array_from_id(const ResourceId* resource_ids,
                               GBitmap* target_array[],
                               unsigned resources_count);

/** Create a layer with initialized custom data. */
Layer*
layer_create_with_init_data(GRect layer_rect,
                            size_t data_size,
                            layer_data_init_t layer_data_init);

#endif

