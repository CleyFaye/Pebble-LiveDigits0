/** @file
 * Utility code
 *
 * Implementation of utility code.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

// ResourceId
// GBitmap
// gbitmap_create_with_resource()
#include <pebble.h>
// Associated header
#include "utils.h"

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

void
load_bitmap_into_array_from_id(const ResourceId* resource_ids,
                               GBitmap* target_array[],
                               unsigned resources_count)
{
    for (unsigned index = 0;
         index < resources_count;
         ++index) {
        target_array[index] = gbitmap_create_with_resource(resource_ids[index]);
    }
}

Layer*
layer_create_with_init_data(GRect layer_rect,
                            size_t data_size,
                            layer_data_init_t layer_data_init)
{
    Layer* result = layer_create_with_data(layer_rect,
                                           data_size);

    if (!result) {
        return NULL;
    }

    layer_data_init(layer_get_data(result));
    return result;
}

