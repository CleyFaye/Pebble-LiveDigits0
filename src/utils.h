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

// ResourceId
// GBitmap
#include <pebble.h>

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
void load_bitmap_into_array_from_id(const ResourceId* resource_ids,
                                    GBitmap* target_array[],
                                    unsigned resources_count);

#endif

